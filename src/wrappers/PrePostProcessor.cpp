#include <emscripten/bind.h>
#include "anira/PrePostProcessor.h"

using namespace emscripten;

// ------ PrePostProcessor ----

EMSCRIPTEN_BINDINGS(PrePostProcessorBindings) {
    class_<anira::PrePostProcessor>("PrePostProcessor")
    .smart_ptr<std::shared_ptr<anira::PrePostProcessor>>("PrePostProcessor")
        .constructor<anira::InferenceConfig&>()
        .function("pre_process", &anira::PrePostProcessor::pre_process)
        .function("post_process", &anira::PrePostProcessor::post_process)
        .function("set_input", &anira::PrePostProcessor::set_input)
        .function("set_output", &anira::PrePostProcessor::set_output)
        .function("get_input", &anira::PrePostProcessor::get_input)
        .function("get_output", &anira::PrePostProcessor::get_output)
        .function("pop_samples_from_buffer", select_overload<void(anira::RingBuffer&, anira::BufferF&, size_t)>(&anira::PrePostProcessor::pop_samples_from_buffer))
        .function("pop_samples_from_buffer_window", select_overload<void(anira::RingBuffer&, anira::BufferF&, size_t, size_t)>(&anira::PrePostProcessor::pop_samples_from_buffer))
        .function("pop_samples_from_buffer_window_offset", select_overload<void(anira::RingBuffer&, anira::BufferF&, size_t, size_t, size_t)>(&anira::PrePostProcessor::pop_samples_from_buffer))
        .function("push_samples_to_buffer", &anira::PrePostProcessor::push_samples_to_buffer);
}