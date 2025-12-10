import { defineConfig } from 'vite'
import { viteStaticCopy } from 'vite-plugin-static-copy'

export default defineConfig({
  base: '/anira-web/',
  assetsInclude: ['**/*.wasm'],
  plugins: [
    viteStaticCopy({
      targets: [
        {
          src: 'node_modules/coi-serviceworker/coi-serviceworker.min.js',
          dest: '.',
        },
      ],
    }),
  ],
  build: {
    target: 'esnext',
    assetsInlineLimit: 0, // Never inline WASM files
    rollupOptions: {
      input: {
        main: './index.html',
        demo: './demo.html',
        benchmark: './benchmark.html',
      },
      output: {
        format: 'es',
      },
    },
  },
  worker: {
    format: 'es',
  },
  server: {
    headers: {
      'Cross-Origin-Embedder-Policy': 'require-corp',
      'Cross-Origin-Opener-Policy': 'same-origin',
    },
  },
  preview: {
    headers: {
      'Cross-Origin-Embedder-Policy': 'require-corp',
      'Cross-Origin-Opener-Policy': 'same-origin',
    },
  },
})
