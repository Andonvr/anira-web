import { AniraModule, type InferenceHandler } from 'anira-web'

// Setting up Anira Inference Handler
const aniraSetup = async (sampleRate: number) => {
  // Initialize the Anira module
  const aniraModule = await AniraModule()

  // Fetch Model
  const res = await fetch('simple_gain_network_stereo.onnx')
  if (!res.ok) {
    throw new Error('Failed to load model')
  }
  const modelBuffer = await res.arrayBuffer()

  // Create ModelData from the buffer
  const modelData = aniraModule.ModelData.createFromBuffer(new Uint8Array(modelBuffer))
  if (!modelData) {
    throw new Error('Failed to create ModelData from buffer')
  }
  // Set up a vector for ModelData
  const vectorModelData = aniraModule.makeVectorModelData([modelData])

  // Create Tensor Shape
  const inputShape = aniraModule.TensorShape.createTensorShapeList([[1, 2, 512], [1]])
  const outputShape = aniraModule.TensorShape.createTensorShapeList([[1, 2, 512], [1]])
  const tensorShape = new aniraModule.TensorShape(
    inputShape,
    outputShape,
    aniraModule.InferenceBackend.ONNX
  )

  // Set up a vector for TensorShape
  const vectorTensorShape = aniraModule.makeVectorTensorShape([tensorShape])

  // Create InferenceConfig
  const inferenceConfig = new aniraModule.InferenceConfig(
    vectorModelData,
    vectorTensorShape,
    new aniraModule.ProcessingSpec(
      aniraModule.makeVectorSizeT([2, 1]),
      aniraModule.makeVectorSizeT([2, 1]),
      aniraModule.makeVectorSizeT([512, 0]),
      aniraModule.makeVectorSizeT([512, 0])
    ),
    5,
    1
  )

  if (!inferenceConfig) {
    throw new Error('Failed to create InferenceConfig')
  }

  // Create PrePostProcessor
  const ppProcessor = new aniraModule.PrePostProcessor(inferenceConfig)
  ppProcessor.set_input(1, 1, 0)

  // Create HostAudioConfig
  const hostAudioConfig = new aniraModule.HostConfig(1024, sampleRate, false, 0)

  // Create InferenceHandler
  const inferenceHandler = new aniraModule.InferenceHandler(ppProcessor, inferenceConfig)
  if (!inferenceHandler) {
    throw new Error('Failed to create InferenceHandler')
  }
  inferenceHandler.setInferenceBackend(aniraModule.InferenceBackend.ONNX)
  inferenceHandler.prepareWithAudioConfig(hostAudioConfig)

  return {
    inferenceHandler,
    hostAudioConfig,
    ppProcessor,
    inferenceConfig,
    vectorModelData,
    vectorTensorShape,
    modelData,
    tensorShape,
    aniraModule,
  }
}

// Initialize Anira Audio Node
const initAniraAudioNode = async (
  aniraModule: AniraModule,
  inferenceHandler: InferenceHandler,
  context: AudioContext
) => {
  // Initialize the Anira Audio module with the AudioContext
  await new Promise<void>((resolve) => {
    aniraModule._module_init(
      aniraModule.emscriptenRegisterAudioObject(context),
      aniraModule.addFunction(resolve, 'vi')
    )
  })

  const audioWorkletNode = await new Promise<AudioWorkletNode>(async (resolve) => {
    aniraModule.node_init(
      inferenceHandler,
      aniraModule.addFunction(
        (audioWorkletNodeHandle: any) =>
          resolve(aniraModule.emscriptenGetAudioObject(audioWorkletNodeHandle)),
        'vip'
      )
    )
  })

  return audioWorkletNode
}

const main = async () => {
  // Resume context (was suspended)
  const ctx = new AudioContext()

  // Setup Anira
  const { aniraModule, inferenceHandler, ppProcessor } = await aniraSetup(ctx.sampleRate)
  // Setup Anira Node
  const audioWorkletNode = await initAniraAudioNode(aniraModule, inferenceHandler, ctx)

  // Chains the Audio Graph
  const audio = new Audio('vibes.mp3')
  const sourceNode = ctx.createMediaElementSource(audio)
  sourceNode.connect(audioWorkletNode).connect(ctx.destination)

  // ------- Setup UI -------

  // Status
  document.getElementById('ctrl-btn')!.removeAttribute('disabled')

  // Setup audio controls
  document.getElementById('ctrl-btn')!.onclick = async () => {
    if (audio.paused) {
      ctx.resume()
      audio.play()
      document.getElementById('ctrl-btn')!.textContent = 'Pause'
    } else {
      audio.pause()
      document.getElementById('ctrl-btn')!.textContent = 'Play'
    }
  }

  // Setup gain control
  document.getElementById('gain-slider')!.oninput = (event) => {
    const gainValue = parseFloat((event.target as HTMLInputElement).value)
    ppProcessor.set_input(gainValue, 1, 0)

    const gainText = `Gain: ${gainValue.toFixed(2)}`
    document.getElementById('gain-label')!.textContent = gainText
  }
}

await main()
