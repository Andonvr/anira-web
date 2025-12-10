# ==============================================================================
# Set up Anira
# ==============================================================================

include(FetchContent)

set(ANIRA_WITH_LIBTORCH OFF CACHE BOOL "Disable LibTorch backend")
set(ANIRA_WITH_PLACEHOLDER OFF CACHE BOOL "Disable Placeholder backend")
set(ANIRA_WITH_TFLITE OFF CACHE BOOL "Disable TensorFlow Lite backend")
set(ANIRA_WITH_ONNXRUNTIME ON CACHE BOOL "Disable ONNX Runtime backend")
if(BUILD_BENCHMARK)
  set(ANIRA_WITH_BENCHMARK ON CACHE BOOL "Enable Anira Benchmark")
else()
  set(ANIRA_WITH_BENCHMARK OFF CACHE BOOL "Enable Anira Benchmark")
endif()

if(LOCAL_ANIRA)
  message(STATUS "Using local Anira module")
  if(NOT EXISTS ${CMAKE_SOURCE_DIR}/modules/anira)
    message(FATAL_ERROR "Local Anira module not found in ${CMAKE_SOURCE_DIR}/modules/anira. Clone it first, or don't use LOCAL_ANIRA.")
  endif()
  add_subdirectory(${CMAKE_SOURCE_DIR}/modules/anira)
else()
  message(STATUS "Fetching Anira from GitHub repository")
  FetchContent_Declare(anira
    GIT_REPOSITORY https://github.com/Andonvr/anira.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(anira)
endif()