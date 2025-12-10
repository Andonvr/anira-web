// Main module export
import MainModuleFactory, * as AllExports from '../wasm/AniraWeb'
// Import WASM file so Vite knows to bundle it
import wasmUrl from '../wasm/AniraWeb.wasm?url'
import jsUrl from '../wasm/AniraWeb.js?url'

export * from '../wasm/AniraWeb'
export type AniraModule = AllExports.MainModule

// Export factory with WASM locateFile override
export const AniraModule = (config?: any) =>
  MainModuleFactory({
    ...config,
    locateFile: (path: string) => {
      if (path.endsWith('.wasm')) {
        return wasmUrl
      }
      if (path.endsWith('.js')) {
        return jsUrl
      }
      return (config as any)?.locateFile?.(path) ?? path
    },
  })
