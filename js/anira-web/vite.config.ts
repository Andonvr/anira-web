import { defineConfig } from 'vite'
import path from 'path'
import dts from 'vite-plugin-dts'
import { viteStaticCopy } from 'vite-plugin-static-copy'

export default defineConfig({
  plugins: [
    viteStaticCopy({
      targets: [
        {
          src: 'wasm/*.wasm',
          dest: 'wasm',
        },
        {
          src: 'wasm/*.js',
          dest: 'wasm',
        },
      ],
    }),
    dts({
      include: ['src/**/*'],
      outDir: 'dist',
    }),
  ],
  build: {
    lib: {
      entry: path.resolve(__dirname, 'src/index.ts'),
      name: 'anira-web',
      fileName: () => 'index.js',
      formats: ['es'],
    },
    minify: false,
    target: 'esnext',
    outDir: 'dist',
    rollupOptions: {
      external: (id) => {
        // Externalize both WASM files and emscripten-generated JS wrappers
        if (id.includes('/wasm/')) return true
        if (id.endsWith('.wasm')) return true
        return false
      },
      output: {
        paths: (id) => {
          // Rewrite paths for wasm imports to be relative to the dist folder
          if (id.includes('/wasm/')) {
            return id.replace(/.*\/wasm\//, './wasm/')
          }
          return id
        },
      },
    },
  },
})
