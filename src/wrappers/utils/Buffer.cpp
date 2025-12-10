#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "anira/utils/Buffer.h"

using namespace emscripten;

// ------ BufferF ----

uintptr_t getReadPointer(
    anira::BufferF& bufferF,
    size_t channel
) {
    return reinterpret_cast<uintptr_t>(bufferF.get_read_pointer(channel));
}
uintptr_t getReadPointerAt(
    anira::BufferF& bufferF,
    size_t channel, 
    size_t sample_index
) {
    return reinterpret_cast<uintptr_t>(bufferF.get_read_pointer(channel, sample_index));
}

uintptr_t getWritePointer(
    anira::BufferF& bufferF,
    size_t channel
) {
    return reinterpret_cast<uintptr_t>(bufferF.get_write_pointer(channel));
}
uintptr_t getWritePointerAt(
    anira::BufferF& bufferF,
    size_t channel, 
    size_t sample_index
) {
    return reinterpret_cast<uintptr_t>(bufferF.get_write_pointer(channel, sample_index));
}

std::vector<uintptr_t> getArrayOfReadPointers(anira::BufferF& bufferF) {
    std::vector<uintptr_t> result;
    const float* const* ptrs = bufferF.get_array_of_read_pointers();
    size_t numChannels = bufferF.get_num_channels();
    result.reserve(numChannels);
    for (size_t i = 0; i < numChannels; ++i) {
        result.push_back(reinterpret_cast<uintptr_t>(ptrs[i]));
    }
    return result;
}

std::vector<uintptr_t> getArrayOfWritePointers(anira::BufferF& bufferF) {
    std::vector<uintptr_t> result;
    float* const* ptrs = bufferF.get_array_of_write_pointers();
    size_t numChannels = bufferF.get_num_channels();
    result.reserve(numChannels);
    for (size_t i = 0; i < numChannels; ++i) {
        result.push_back(reinterpret_cast<uintptr_t>(ptrs[i]));
    }
    return result;
}

void swapDataWithRawPointerWrapper(anira::BufferF& buffer, uintptr_t rawPointer, size_t size) {
    float* rawPtr = reinterpret_cast<float*>(rawPointer);
    buffer.swap_data(rawPtr, size);
}

uintptr_t getDataPointer(anira::BufferF& buffer) {
    return reinterpret_cast<uintptr_t>(buffer.data());
}

EMSCRIPTEN_BINDINGS(BufferFBindings) {
    class_<anira::BufferF>("BufferF")
        .smart_ptr<std::shared_ptr<anira::BufferF>>("BufferF")
        .constructor<>()
        .constructor<size_t, size_t>()
        .function("resize", &anira::BufferF::resize)
        .function("getNumChannels", &anira::BufferF::get_num_channels)
        .function("getNumSamples", &anira::BufferF::get_num_samples)
        .function("getReadPointer", &getReadPointer)
        .function("getReadPointerAt", &getReadPointerAt)
        .function("getWritePointer", &getWritePointer)
        .function("getWritePointerAt", &getWritePointerAt)
        .function("getArrayOfReadPointers", &getArrayOfReadPointers)
        .function("getArrayOfWritePointers", &getArrayOfWritePointers)
        .function("data", &getDataPointer)
        .function("swapDataWithBuffer", select_overload<void(anira::BufferF&)>(&anira::BufferF::swap_data))
        .function("swapDataWithRawPointer", &swapDataWithRawPointerWrapper)
        .function("resetChannelPtr", &anira::BufferF::reset_channel_ptr)
        .function("getSample", &anira::BufferF::get_sample)
        .function("setSample", &anira::BufferF::set_sample)
        .function("clear", &anira::BufferF::clear);
        // .function("getMemoryBlock", &anira::BufferF::get_memory_block, return_value_policy::reference())
        // .function("swapDataWithMemoryBlock", select_overload<void(anira::MemoryBlock<float>&)>(&anira::BufferF::swap_data))
}