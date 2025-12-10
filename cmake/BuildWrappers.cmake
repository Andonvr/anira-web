# ==============================================================================
# Build Wrappers
# ==============================================================================

if(NOT WASM)
  message(FATAL_ERROR "Cannot build JS wrappers without Emscripten toolchain")
endif()

function(build_wrappers TARGET_NAME OUTPUT_FOLDER)
  # Set flags if Debug
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEBUG_FLAGS "-O0 -gsource-map")
  else()
    set(DEBUG_FLAGS "")
  endif()

  set(LINK_FLAGS "\
    --bind \
    --no-entry \
    --emit-tsd=${OUTPUT_FOLDER}/${TARGET_NAME}.d.ts \
    -pthread \
    -s AUDIO_WORKLET=1 \
    -s WASM_WORKERS=32 \
    -s PTHREAD_POOL_SIZE=32 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ALLOW_TABLE_GROWTH=1 \
    -s MALLOC=emmalloc \
    -s EXPORT_ES6=1 \
    -s MODULARIZE=1 \
    -s ENVIRONMENT=web,worker,shell \
    -s EXPORTED_FUNCTIONS='_module_init' \
    -s EXPORTED_RUNTIME_METHODS=['emscriptenRegisterAudioObject','emscriptenGetAudioObject','addFunction'] \
    -s NO_DISABLE_EXCEPTION_CATCHING \
    -s TOTAL_STACK=10MB \
  ")

  add_executable(${TARGET_NAME} 
    src/wrappers/utils/Buffer.cpp
    src/wrappers/utils/HostConfig.cpp
    src/wrappers/utils/InferenceBackend.cpp
    src/wrappers/utils/RingBuffer.cpp
    src/wrappers/InferenceConfig.cpp
    src/wrappers/InferenceHandler.cpp
    src/wrappers/PrePostProcessor.cpp
    src/wrappers/vectors.cpp
    src/audio_worklet.cpp
  )
  target_link_libraries(${TARGET_NAME} PUBLIC anira::anira)
  target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

  set_target_properties(${TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "${EM_CFLAGS}"
    LINK_FLAGS "${DEBUG_FLAGS} ${LINK_FLAGS}"
    OUTPUT_NAME ${TARGET_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_FOLDER}
  )
endfunction()

build_wrappers("AniraWeb" "${CMAKE_SOURCE_DIR}/js/anira-web/wasm")