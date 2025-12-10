# ==============================================================================
# Clone the required model repositories for the benchmarks
# ==============================================================================

set(MODEL_REPOSITORIES
        "https://github.com/faressc/GuitarLSTM.git hybrid-nn/GuitarLSTM"
        "https://github.com/faressc/steerable-nafx.git cnn/steerable-nafx"
        "https://github.com/vackva/stateful-lstm.git stateful-rnn/stateful-lstm"
        "https://github.com/anira-project/example-models.git model-pool/example-models"
)

find_package(Git QUIET)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "Git not found")
endif()

foreach(repo IN LISTS MODEL_REPOSITORIES)
    string(REPLACE " " ";" SPLIT_REPO_DETAILS ${repo})
    list(GET SPLIT_REPO_DETAILS 0 REPO_URL)
    list(GET SPLIT_REPO_DETAILS 1 INSTALL_PATH)

    set(GIT_CLONE_DIR "${CMAKE_SOURCE_DIR}/src/benchmarks/models/${INSTALL_PATH}")

    if(NOT EXISTS "${GIT_CLONE_DIR}")
        message(STATUS "Cloning ${REPO_URL} into ${GIT_CLONE_DIR}")
        execute_process(
                COMMAND ${GIT_EXECUTABLE} clone ${REPO_URL} ${GIT_CLONE_DIR}
                RESULT_VARIABLE GIT_CLONE_RESULT
        )
        if(NOT GIT_CLONE_RESULT EQUAL "0")
            message(FATAL_ERROR "Git clone of ${REPO_URL} failed with ${GIT_CLONE_RESULT}")
        endif()
    endif()
endforeach()

# Using CACHE PATH to set the default path for the models, since PARENT_SCOPE is only propagated one level up. Cache variables are global.
set(GUITARLSTM_MODELS_PATH_TENSORFLOW "${CMAKE_SOURCE_DIR}/src/benchmarks/models/hybrid-nn/GuitarLSTM/tensorflow-version/models/" CACHE PATH "Path to the GuitarLSTM TensorFlow models")
set(GUITARLSTM_MODELS_PATH_PYTORCH "${CMAKE_SOURCE_DIR}/src/benchmarks/models/hybrid-nn/GuitarLSTM/pytorch-version/models/" CACHE PATH "Path to the GuitarLSTM PyTorch models")

set(STEERABLENAFX_MODELS_PATH_TENSORFLOW "${CMAKE_SOURCE_DIR}/src/benchmarks/models/cnn/steerable-nafx/models/" CACHE PATH "Path to the SteerableNAFX TensorFlow models")
set(STEERABLENAFX_MODELS_PATH_PYTORCH "${CMAKE_SOURCE_DIR}/src/benchmarks/models/cnn/steerable-nafx/models/" CACHE PATH "Path to the SteerableNAFX PyTorch models")

set(STATEFULLSTM_MODELS_PATH_TENSORFLOW "${CMAKE_SOURCE_DIR}/src/benchmarks/models/stateful-rnn/stateful-lstm/models/" CACHE PATH "Path to the StatefulLSTM TensorFlow models")
set(STATEFULLSTM_MODELS_PATH_PYTORCH "${CMAKE_SOURCE_DIR}/src/benchmarks/models/stateful-rnn/stateful-lstm/models/" CACHE PATH "Path to the StatefulLSTM PyTorch models")

set(SIMPLEGAIN_MODEL_PATH "${CMAKE_SOURCE_DIR}/src/benchmarks/models/model-pool/example-models/SimpleGainNetwork/models/" CACHE PATH "Path to the SimpleGainNetwork models")

add_compile_definitions(
  GUITARLSTM_MODELS_PATH_TENSORFLOW="${GUITARLSTM_MODELS_PATH_TENSORFLOW}"
  GUITARLSTM_MODELS_PATH_PYTORCH="${GUITARLSTM_MODELS_PATH_PYTORCH}"
  STEERABLENAFX_MODELS_PATH_TENSORFLOW="${STEERABLENAFX_MODELS_PATH_TENSORFLOW}"
  STEERABLENAFX_MODELS_PATH_PYTORCH="${STEERABLENAFX_MODELS_PATH_PYTORCH}"
  STATEFULLSTM_MODELS_PATH_TENSORFLOW="${STATEFULLSTM_MODELS_PATH_TENSORFLOW}"
  STATEFULLSTM_MODELS_PATH_PYTORCH="${STATEFULLSTM_MODELS_PATH_PYTORCH}"
  SIMPLEGAIN_MODEL_PATH="${SIMPLEGAIN_MODEL_PATH}"
)

# ==============================================================================
# Build Benchmarks
# ==============================================================================

set(BENCHMARK_TARGET_NAME "Benchmarks")
message(STATUS "Building Anira Benchmark...")
if(WASM)
  set(OUTPUT_FOLDER ${CMAKE_SOURCE_DIR}/js/anira-web-benchmarks/wasm)
  # Emscripten specific settings
  set(LINK_FLAGS "\
      --bind \
      --emit-tsd=${OUTPUT_FOLDER}/${BENCHMARK_TARGET_NAME}.d.ts \
      -pthread \
      -s PTHREAD_POOL_SIZE=4 \
      -s PROXY_TO_PTHREAD \
      -s MALLOC=mimalloc \
      -s INITIAL_MEMORY=512MB \
      -s ALLOW_MEMORY_GROWTH=1 \
      -s MAXIMUM_MEMORY=2GB \
      -s EXPORT_ES6=1 \
      -s MODULARIZE=1 \
      -s FORCE_FILESYSTEM=1 \
      -s STACK_SIZE=2MB \
      -s NO_DISABLE_EXCEPTION_CATCHING \
      --embed-file ${GUITARLSTM_MODELS_PATH_TENSORFLOW}@/${GUITARLSTM_MODELS_PATH_TENSORFLOW} \
      --embed-file ${GUITARLSTM_MODELS_PATH_PYTORCH}@/${GUITARLSTM_MODELS_PATH_PYTORCH} \
      --embed-file ${STEERABLENAFX_MODELS_PATH_TENSORFLOW}@/${STEERABLENAFX_MODELS_PATH_TENSORFLOW} \
      --embed-file ${STEERABLENAFX_MODELS_PATH_PYTORCH}@/${STEERABLENAFX_MODELS_PATH_PYTORCH} \
      --embed-file ${STATEFULLSTM_MODELS_PATH_TENSORFLOW}@/${STATEFULLSTM_MODELS_PATH_TENSORFLOW} \
      --embed-file ${STATEFULLSTM_MODELS_PATH_PYTORCH}@/${STATEFULLSTM_MODELS_PATH_PYTORCH} \
      --embed-file ${SIMPLEGAIN_MODEL_PATH}@/${SIMPLEGAIN_MODEL_PATH} \
  ")
else()
  set(OUTPUT_FOLDER ${CMAKE_BINARY_DIR})
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  set(DEBUG_FLAGS "-O0 -gsource-map")
else()
  set(DEBUG_FLAGS "")
endif()

add_executable(${BENCHMARK_TARGET_NAME} src/benchmarks/benchmark.cpp)
target_link_libraries(${BENCHMARK_TARGET_NAME} PUBLIC anira::anira)
target_compile_features(${BENCHMARK_TARGET_NAME} PUBLIC cxx_std_20)

set_target_properties(${BENCHMARK_TARGET_NAME} PROPERTIES
  COMPILE_FLAGS "${EM_CFLAGS}"
  LINK_FLAGS "${DEBUG_FLAGS} ${LINK_FLAGS}"
  OUTPUT_NAME ${BENCHMARK_TARGET_NAME}
  RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_FOLDER}
)