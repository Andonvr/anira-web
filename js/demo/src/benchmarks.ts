import { AniraBenchmark } from 'anira-web-benchmarks'

const downloadBlob = (blob: Blob, filename: string) => {
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = filename
  document.body.appendChild(a)
  a.click()
  document.body.removeChild(a)
  URL.revokeObjectURL(url)
}

const getBrowserName = () => {
  const ua = navigator.userAgent

  if (ua.includes('Firefox')) return 'Firefox'
  if (ua.includes('Edg')) return 'Microsoft Edge'
  if (ua.includes('Chrome')) return 'Chrome'
  if (ua.includes('Safari')) return 'Safari'
  if (ua.includes('Opera') || ua.includes('OPR')) return 'Opera'

  return 'Unknown'
}

const runBenchmark = async () => {
  // Callback to resolve when benchmarking is done
  let benchmarksDoneCallback = () => {}
  const benchmarkingDonePromise = new Promise<void>((resolve) => {
    benchmarksDoneCallback = resolve
  })

  const output: string[] = []
  // Run benchmarks
  await AniraBenchmark({
    printErr: (str: string) => {
      output.push(`Error: ${str}`)
      console.error('Error:', str)
    },
    print: (str: string) => {
      // Special message to indicate the end of the benchmark
      if (str.startsWith('DEBUG:')) {
        if (str === 'DEBUG: DONE') {
          benchmarksDoneCallback()
        }
        console.log(str)
        return
      }
      output.push(str)
    },
  })

  // Wait for the benchmarking to finish
  await benchmarkingDonePromise
  console.log('Benchmarking done!')

  // Turn output into a downloadable blob (.log file)
  return new Blob([output.join('\n')], { type: 'text/plain' })
}

const main = () => {
  const startBenchmarkBtn = document.getElementById(
    'start-benchmark-btn'
  ) as HTMLButtonElement
  const downloadResultsBtn = document.getElementById(
    'download-results-btn'
  ) as HTMLButtonElement
  if (!startBenchmarkBtn) throw new Error('Start Benchmark button not found')
  if (!downloadResultsBtn) throw new Error('Download Results button not found')

  startBenchmarkBtn.onclick = async () => {
    startBenchmarkBtn.disabled = true
    const resultBlob = await runBenchmark()

    downloadResultsBtn.disabled = false
    downloadResultsBtn.onclick = () => {
      downloadBlob(resultBlob, `${getBrowserName()}-results.log`)
    }
  }
}
main()
