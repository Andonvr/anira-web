# Anira Web

Anira-Web is a WebAssembly port of [Anira](https://github.com/anira-project/anira), a library for neural inference of real-time audio signals.
This repo provides:

- Build scripts for compiling Anira to WebAssembly using Emscripten.
- An Embind-based binding layer for accessing the Anira API in JavaScript.
- Code to instantiate an Audio Worklet that uses Anira for real-time audio processing in the browser
- A benchmark suite to measure the performance of Anira-Web, either be compiled to WebAssembly for the anira-web-benchmarks package, or natively. This is useful for comparing the performance of Anira-Web against native Anira builds.
- An npm package that bundles aforementioned Anira-Web-Benchmarks.
- An npm package that bundles Anira-Web for easy use in web projects.
- An example for both the usage of Anira-Web, and for running the benchmark suite in the browser.

This repo does not contain the Anira source code itself. That one is located in [my fork of the Anira project](https://github.com/Andonvr/anira) (see the setup instructions below).

## Setup Instructions

- You can either use your own install of the Emscripten SDK, or set up a local copy using `./setup_emscripten.sh`.
  This project was tested using Emscripten SDK version 4.0.11.

- Anira will either be cloned can either be cloned before building, using the `./setup_anira.sh` script, or it will be cloned automatically during the build process.

## Building the Anira-Web WASM Modules

- Reference the `CMakePresets.json` file for available build presets.
  | Preset Name | Description |
  |-------------|-------------|
  | web | Builds for the web with benchmarks and wrappers enabled. |
  | web-wrappers | Builds for the web with wrappers enabled. |
  | web-benchmarks | Builds for the web with benchmarks enabled. |
  | native-benchmarks | Builds native benchmarks for performance comparison. |

- Alternatively, specify your toolchainFile and necessary build variables yourself.

## Building the npm package

- After building the WASM module, navigate to the `js` directory. It is a pnpm monorepo, containing a package for using Anira, a package for running the benchmarks, and a demo page.
- Navigate to the the `js` directory
- run `pnpm install` to install dependencies.
- run `pnpm run build` to build the npm packages.
- run `pnpm run dev` to start the usage example in a local development server.
