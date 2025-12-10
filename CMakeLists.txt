cmake_minimum_required(VERSION 3.15)
project(anira_web VERSION 1.0 LANGUAGES CXX)

option(BUILD_BENCHMARK "Build Benchmarks" OFF)
option(BUILD_WRAPPERS "Build Wrappers" OFF)
option(LOCAL_ANIRA "Use local Anira module" OFF)

include(cmake/DetectEmscripten.cmake)
include(cmake/SetupAnira.cmake)

if(NOT BUILD_BENCHMARK AND NOT BUILD_WRAPPERS)
  message(FATAL_ERROR "At least one of BUILD_BENCHMARK or BUILD_WRAPPERS must be enabled.")
endif()

if(BUILD_BENCHMARK)
  include(cmake/BuildBenchmarks.cmake)
endif()
if(BUILD_WRAPPERS)
  include(cmake/BuildWrappers.cmake)
endif()