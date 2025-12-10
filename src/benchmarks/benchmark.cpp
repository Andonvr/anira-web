#include "anira/InferenceConfig.h"
#include <benchmark/benchmark.h>
#include <anira/anira.h>
#include <anira/benchmark.h>
#include <vector>
#include <gtest/gtest.h>

#include "model_configs/cnn/CNNConfig.h"
#include "model_configs/cnn/CNNPrePostProcessor.h"
#include "model_configs/hybrid-nn/HybridNNConfig.h"
#include "model_configs/hybrid-nn/HybridNNPrePostProcessor.h"
#include "ClearCustomProcessor.h"

/* ============================================================ *
 * ========================= Configs ========================== *
 * ============================================================ */

#define NUM_ITERATIONS 50
#define NUM_REPETITIONS 10
#define PERCENTILE 0.999
#define SAMPLE_RATE 44100

std::vector<int> buffer_sizes = {64, 1024, 8192};
std::vector<anira::InferenceBackend> inference_backends = {
    anira::InferenceBackend::ONNX,
    anira::InferenceBackend::CUSTOM
};
std::vector<anira::InferenceConfig> inference_configs = {cnn_config, hybridnn_config};
anira::InferenceConfig inference_config;


void adapt_config(anira::InferenceConfig& inference_config, int buffer_size, int model) {
    if (model == 0) {
        int receptive_field = 13332;
        int input_size = buffer_size + receptive_field;
        int output_size = buffer_size;
        inference_config.set_tensor_input_shape({{1, 1, input_size}});
        inference_config.set_tensor_output_shape({{1, 1, output_size}});
        inference_config.clear_processing_spec();
        inference_config.update_processing_spec();
        inference_config.set_preprocess_input_size(std::vector<size_t>{static_cast<size_t>(input_size - receptive_field)});
    } else if (model == 1) {
        inference_config.set_tensor_input_shape({{buffer_size, 1, 150}});
        inference_config.set_tensor_output_shape({{buffer_size, 1}});
        inference_config.clear_processing_spec();
        inference_config.update_processing_spec();
        inference_config.set_preprocess_input_size(std::vector<size_t>{static_cast<size_t>(buffer_size)});
    }
}

// define the buffer sizes, backends and model configs to be used in the benchmark and the backends to be used
static void Arguments(::benchmark::internal::Benchmark* b) {
    for (int i = 0; i < buffer_sizes.size(); ++i)
        for (int j = 0; j < inference_configs.size(); ++j)
            for (int k = 0; k < inference_backends.size(); ++k)
                // ONNX backend does not support stateful RNN
                if (!(j == 2 && k == 1))
                    b->Args({buffer_sizes[i], j, k});
}

/* ============================================================ *
 * ================== BENCHMARK DEFINITIONS =================== *
 * ============================================================ */

const int TOTAL_BENCHMARK_RUNS = NUM_REPETITIONS * buffer_sizes.size() * inference_backends.size() * inference_configs.size();
static int current_repetition = 0;
void log_benchmark_run(int buffer_size, int model, int backend) {
    ++current_repetition;
    printf("DEBUG: Benchmark run %d/%d: Buffer size=%d, Model=%d, Backend=%d\n",
           current_repetition, TOTAL_BENCHMARK_RUNS, buffer_size, model, backend);
}

typedef anira::benchmark::ProcessBlockFixture ProcessBlockFixture;

BENCHMARK_DEFINE_F(ProcessBlockFixture, BM_ADVANCED)(::benchmark::State& state) {
    log_benchmark_run(state.range(0), state.range(1), state.range(2));
    
    // The buffer size return in get_buffer_size() is populated by state.range(0) param of the google benchmark
    anira::HostConfig host_config = {static_cast<float>(get_buffer_size()), SAMPLE_RATE};

    inference_config = inference_configs[state.range(1)];

    adapt_config(inference_config, get_buffer_size(), state.range(1));

    anira::PrePostProcessor *my_pp_processor;

    if (state.range(1) == 0) {
        my_pp_processor = new CNNPrePostProcessor(inference_config);
    } else if (state.range(1) == 1) {
        my_pp_processor = new HybridNNPrePostProcessor(inference_config);
    }

    ClearCustomProcessor clear_custom_processor(inference_config);

    m_inference_handler = std::make_unique<anira::InferenceHandler>(*my_pp_processor, inference_config, clear_custom_processor);
    m_inference_handler->prepare(host_config);
    m_inference_handler->set_inference_backend(inference_backends[state.range(2)]);

    m_buffer = std::make_unique<anira::Buffer<float>>(inference_config.get_preprocess_input_channels()[0], host_config.m_buffer_size);

    initialize_repetition(inference_config, host_config, inference_backends[state.range(2)]);

    for (auto _ : state) {
        push_random_samples_in_buffer(host_config);

        initialize_iteration();

        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        
        m_inference_handler->process(m_buffer->get_array_of_write_pointers(), get_buffer_size());

        while (!buffer_processed()) {
            std::this_thread::sleep_for(std::chrono::nanoseconds (10));
        }
        
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        interation_step(start, end, state);
    }
    repetition_step();

    delete my_pp_processor;
}

// /* ============================================================ *
//  * ================== BENCHMARK REGISTRATION ================== *
//  * ============================================================ */

BENCHMARK_REGISTER_F(ProcessBlockFixture, BM_ADVANCED)
->Unit(benchmark::kMillisecond)
->Iterations(NUM_ITERATIONS)->Repetitions(NUM_REPETITIONS)
->Apply(Arguments)
->ComputeStatistics("min", anira::calculate_min)
->ComputeStatistics("max", anira::calculate_max)
->ComputeStatistics("percentile", [](const std::vector<double>& v) -> double {
    return anira::calculate_percentile(v, PERCENTILE);
  })
->DisplayAggregatesOnly(false)
->UseManualTime();

// If this is using Emscripten, call the Emscripten benchmark main function
#ifdef __EMSCRIPTEN__

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    
    printf("DEBUG: DONE\n");
}

#else
TEST(Benchmark, Advanced){
#if __linux__ || __APPLE__
    pthread_t self = pthread_self();
#elif WIN32
    HANDLE self = GetCurrentThread();
#endif
    anira::HighPriorityThread::elevate_priority(self, true);

    benchmark::RunSpecifiedBenchmarks();
}
#endif