#include <cstdio>
#include <cstdlib>
#include <emscripten/webaudio.h>
#include <pthread.h>
#include <emscripten/bind.h>
#include <anira/InferenceHandler.h>

// Process InferenceHandler in the audio worklet
EM_BOOL anira_process_cb(
    int numInputs, const AudioSampleFrame *inputs,
    int numOutputs, AudioSampleFrame *outputs,
    int numParams, const AudioParamFrame *params, void *userData
) {
    auto anira_processor = static_cast<anira::InferenceHandler*>(userData);
    auto input = inputs[0];
    auto output = outputs[0];

    // Real-time safe audio processing in process callback of your application
    size_t num_channels = input.numberOfChannels;
    size_t num_samples = input.samplesPerChannel;

    // Convert to raw pointers for processing
    std::vector<float*> input_ptrs, output_ptrs;
    for (size_t i = 0; i < num_channels; ++i) {
        input_ptrs.push_back(input.data + i * num_samples);
        output_ptrs.push_back(output.data + i * num_samples);
    }

    // Process once
    anira_processor->process(input_ptrs.data(), num_samples, output_ptrs.data(), num_samples);
    
    return true;
}

// Logic for setting up the audio worklet node
#define AWP_NAME "Test"
typedef void (*js_node_init_cb)(int data, anira::InferenceHandler* InferenceHandler);
typedef void (*js_thread_init_cb)(int data);

uint8_t audioThreadStack[4096];
EMSCRIPTEN_WEBAUDIO_T audio_context;

void worklets_registered_cb(EMSCRIPTEN_WEBAUDIO_T audio_ctx, EM_BOOL success, void *thread_init_cb) {
    if (!success) return;
    audio_context = audio_ctx;
    ((js_thread_init_cb)thread_init_cb)(1);
}

void register_worklets(EMSCRIPTEN_WEBAUDIO_T audio_ctx, EM_BOOL success, void *thread_init_cb) {
    if (!success) return;

    WebAudioParamDescriptor paramDescriptors[] = {};
    WebAudioWorkletProcessorCreateOptions opts = {
        .name = AWP_NAME,
        .numAudioParams = 0,
        .audioParamDescriptors = paramDescriptors
    };

    emscripten_create_wasm_audio_worklet_processor_async(
        audio_ctx,
        &opts,
        worklets_registered_cb,
        thread_init_cb
    );
}

extern "C" {
    /**
    * Initialize the WasWASM module
    *
    * @param audio_ctx Handle of a Web Audio API AudioContext.
    * @param init_js_cb Pointer to JS callback that will be used for exporting an
    *  AudioWorkletNode created as the result of module initialization.
    * @note Should be called once per page load. `audio_ctx` can be obtained in
    *  JS via emscriptenRegisterAudioObject() on a preexisting AudioContext.
    */
    void module_init(
        EMSCRIPTEN_WEBAUDIO_T audio_ctx,
        js_thread_init_cb thread_init_cb
    ) {
        emscripten_start_wasm_audio_worklet_thread_async(
            audio_ctx,
            audioThreadStack,
            sizeof(audioThreadStack),
            register_worklets,
            (void *)thread_init_cb
        );
    }
}

void node_init(anira::InferenceHandler* anira_processor, uintptr_t node_init_cb) {
    js_node_init_cb cb = reinterpret_cast<js_node_init_cb>(node_init_cb);

    if(!audio_context) {
        cb(0, nullptr);
        return;
    };
        
    int output_channels[] = {2};
    EmscriptenAudioWorkletNodeCreateOptions options = {
        .numberOfInputs = 1,
        .numberOfOutputs = 1,
        .outputChannelCounts = output_channels
    };

    auto processor_ptr = anira_processor;
    EMSCRIPTEN_AUDIO_WORKLET_NODE_T node_handle =
        emscripten_create_wasm_audio_worklet_node(
            audio_context,
            AWP_NAME,
            &options,
            anira_process_cb,
            processor_ptr
        );
        
    cb(node_handle, processor_ptr);
}

using namespace emscripten;
EMSCRIPTEN_BINDINGS(audio_worklet) {
    function("node_init", &node_init, allow_raw_pointers());
}