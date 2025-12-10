// Benchmark export
import BenchmarkFactory from '../wasm/Benchmarks'
// Import WASM file so Vite knows to bundle it
import wasmUrl from '../wasm/Benchmarks.wasm?url'
import jsUrl from '../wasm/Benchmarks.js?url'

// Export factory with WASM locateFile override
export const AniraBenchmark = (config?: any) =>
  BenchmarkFactory({
    ...config,
    locateFile: (path: string) => {
      if (path.endsWith('.wasm')) {
        return wasmUrl
      }
      if (path.endsWith('.js')) {
        return jsUrl
      }
      return config?.locateFile?.(path) ?? path
    },
  })
