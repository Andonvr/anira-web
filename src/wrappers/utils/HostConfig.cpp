#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "anira/utils/HostConfig.h"

using namespace emscripten;

// ------ HostConfig ----

EMSCRIPTEN_BINDINGS(HostConfigBindings) {
    class_<anira::HostConfig>("HostConfig")
    .smart_ptr<std::shared_ptr<anira::HostConfig>>("HostConfig")
        .constructor<>()
        .constructor<float, float, bool, size_t>()
        .property("bufferSize", &anira::HostConfig::m_buffer_size)
        .property("sampleRate", &anira::HostConfig::m_sample_rate)
        .property("allowSmallerBuffers", &anira::HostConfig::m_allow_smaller_buffers)
        .property("tensorIndex", &anira::HostConfig::m_tensor_index)
        .function("equals", &anira::HostConfig::operator==)
        .function("notEquals", &anira::HostConfig::operator!=)
        .function("getRelativeBufferSize", &anira::HostConfig::get_relative_buffer_size)
        .function("getRelativeSampleRate", &anira::HostConfig::get_relative_sample_rate);
}
