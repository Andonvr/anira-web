#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "anira/utils/RingBuffer.h"

using namespace emscripten;

// ------ RingBuffer ----

EMSCRIPTEN_BINDINGS(RingBuffer) {
    class_<anira::RingBuffer>("RingBuffer")
        .smart_ptr<std::shared_ptr<anira::RingBuffer>>("RingBuffer")
        .constructor<>()
        .function("initializeWithPositions", &anira::RingBuffer::initialize_with_positions)
        .function("clearWithPositions", &anira::RingBuffer::clear_with_positions)
        .function("pushSample", &anira::RingBuffer::push_sample)
        .function("popSample", &anira::RingBuffer::pop_sample)
        .function("getFutureSample", &anira::RingBuffer::get_future_sample)
        .function("getPastSample", &anira::RingBuffer::get_past_sample)
        .function("getAvailableSamples", &anira::RingBuffer::get_available_samples)
        .function("getAvailablePastSamples", &anira::RingBuffer::get_available_past_samples);
}

