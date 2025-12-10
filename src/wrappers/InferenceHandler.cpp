#include <emscripten/bind.h>
#include "anira/InferenceHandler.h"

using namespace emscripten;

// For "size_t process(float* const* data, size_t num_samples, size_t tensor_index = 0)"
size_t process_in_place(anira::InferenceHandler& inf_handler, const std::vector<std::vector<float>>& data, size_t num_samples, size_t tensor_index) {
    // Convert std::vector<std::vector<float>> to float* const*
    std::vector<float*> input_data_ptrs;
    for (const auto& vec : data) {
        input_data_ptrs.push_back(const_cast<float*>(vec.data()));
    }
    return inf_handler.process(input_data_ptrs.data(), num_samples, tensor_index);
}

// For "size_t process(const float* const* input_data, size_t num_input_samples, float* const* output_data, size_t num_output_samples, size_t tensor_index = 0)"
size_t process_separate_buffers(anira::InferenceHandler& inf_handler, const std::vector<std::vector<float>>& input_data, size_t num_input_samples, const std::vector<std::vector<float>>& output_data, size_t num_output_samples, size_t tensor_index) {
    // Convert std::vector<std::vector<float>> to float* const*
    std::vector<float*> input_data_ptrs;
    for (const auto& vec : input_data) {
        input_data_ptrs.push_back(const_cast<float*>(vec.data()));
    }
    std::vector<float*> output_data_ptrs;
    for (const auto& vec : output_data) {
        output_data_ptrs.push_back(const_cast<float*>(vec.data()));
    }
    return inf_handler.process(input_data_ptrs.data(), num_input_samples, output_data_ptrs.data(), num_output_samples, tensor_index);
}

// For "size_t* process(const float* const* const* input_data, size_t* num_input_samples, float* const* const* output_data, size_t* num_output_samples)"
size_t* process_multi_tensor(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<std::vector<float>>>& input_data,
    std::vector<size_t>& num_input_samples,
    const std::vector<std::vector<std::vector<float>>>& output_data,
    std::vector<size_t>& num_output_samples
) {
    // Convert input_data to float* const* const*
    std::vector<std::vector<float*>> input_tensor_ptrs;
    input_tensor_ptrs.reserve(input_data.size());
    for (const auto& tensor : input_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        input_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    std::vector<float**> input_data_ptrs;
    for (auto& tensor_ptrs : input_tensor_ptrs) {
        input_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Convert output_data to float* const* const*
    std::vector<std::vector<float*>> output_tensor_ptrs;
    output_tensor_ptrs.reserve(output_data.size());
    for (const auto& tensor : output_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        output_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    std::vector<float**> output_data_ptrs;
    for (auto& tensor_ptrs : output_tensor_ptrs) {
        output_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Call the actual function
    return inf_handler.process(
        input_data_ptrs.data(),
        num_input_samples.data(),
        output_data_ptrs.data(),
        num_output_samples.data()
    );
}

// For "void push_data(const float* const* input_data, size_t num_input_samples, size_t tensor_index = 0)"
void push_data_single_tensor(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<float>>& input_data,
    size_t num_samples,
    size_t tensor_index
) {
    std::vector<float*> input_ptrs;
    input_ptrs.reserve(input_data.size());
    for (const auto& vec : input_data) {
        input_ptrs.push_back(const_cast<float*>(vec.data()));
    }
    inf_handler.push_data(input_ptrs.data(), num_samples, tensor_index);
}

// For "void push_data(const float* const* const* input_data, size_t* num_input_samples)"
void push_data_multi_tensor(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<std::vector<float>>>& input_data,
    std::vector<size_t>& num_input_samples
) {
    // Convert input_data to float* const* const*
    std::vector<std::vector<float*>> input_tensor_ptrs;
    input_tensor_ptrs.reserve(input_data.size());
    for (const auto& tensor : input_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        input_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    std::vector<float**> input_data_ptrs;
    for (auto& tensor_ptrs : input_tensor_ptrs) {
        input_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Now call the actual function
    inf_handler.push_data(input_data_ptrs.data(), num_input_samples.data());
}

// For "size_t* pop_data(float* const* const* output_data, size_t* num_output_samples)"
std::vector<size_t> popDataSingleNonBlocking(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<std::vector<float>>>& output_data,
    std::vector<size_t>& num_output_samples
) {
    // Convert output_data to float* const* const*
    std::vector<std::vector<float*>> output_tensor_ptrs;
    output_tensor_ptrs.reserve(output_data.size());
    for (const auto& tensor : output_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        output_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    std::vector<float**> output_data_ptrs;
    output_data_ptrs.reserve(output_tensor_ptrs.size());
    for (auto& tensor_ptrs : output_tensor_ptrs) {
        output_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Call the native function
    size_t* results = inf_handler.pop_data(output_data_ptrs.data(), num_output_samples.data());

    // Convert returned raw pointer (size_t*) to std::vector<size_t>
    // Assuming the length matches output_data.size() (one per tensor)
    std::vector<size_t> result_vec(output_data.size());
    std::memcpy(result_vec.data(), results, output_data.size() * sizeof(size_t));

    return result_vec;
}


// For "size_t* pop_data(float* const* const* output_data, size_t* num_output_samples, std::chrono::steady_clock::time_point wait_until)"
std::vector<size_t> popDataSingleBlocking(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<std::vector<float>>>& output_data,
    std::vector<size_t>& num_output_samples,
    double wait_until_ms // Pass milliseconds from JS side
) {
    // Convert wait_until_ms (epoch milliseconds) → std::chrono::steady_clock::time_point
    auto now = std::chrono::steady_clock::now();
    auto wait_until = now + std::chrono::milliseconds(static_cast<long long>(wait_until_ms));

    // Convert output_data to float* const* const*
    std::vector<std::vector<float*>> output_tensor_ptrs;
    output_tensor_ptrs.reserve(output_data.size());
    for (const auto& tensor : output_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        output_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    std::vector<float**> output_data_ptrs;
    output_data_ptrs.reserve(output_tensor_ptrs.size());
    for (auto& tensor_ptrs : output_tensor_ptrs) {
        output_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Call the native function
    size_t* results = inf_handler.pop_data(output_data_ptrs.data(), num_output_samples.data(), wait_until);

    // Convert returned raw pointer (size_t*) to std::vector<size_t>
    // Assuming the length matches output_data.size() (one per tensor)
    std::vector<size_t> result_vec(output_data.size());
    std::memcpy(result_vec.data(), results, output_data.size() * sizeof(size_t));

    return result_vec;
}

// For "size_t* pop_data(float* const* const* output_data, size_t* num_output_samples)"
std::vector<size_t> popDataMultipleNonBlocking(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<std::vector<float>>>& output_data,
    std::vector<size_t>& num_output_samples
) {
    // Convert output_data (vector[tensor][channel][sample]) -> float* const* const*
    std::vector<std::vector<float*>> output_tensor_ptrs;
    output_tensor_ptrs.reserve(output_data.size());
    for (const auto& tensor : output_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        output_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    // Now create the top-level array of float**
    std::vector<float**> output_data_ptrs;
    output_data_ptrs.reserve(output_tensor_ptrs.size());
    for (auto& tensor_ptrs : output_tensor_ptrs) {
        output_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Call the actual native method
    size_t* results = inf_handler.pop_data(output_data_ptrs.data(), num_output_samples.data());

    // Convert the returned size_t* into a std::vector<size_t>
    std::vector<size_t> result_vec(output_data.size());
    std::memcpy(result_vec.data(), results, output_data.size() * sizeof(size_t));

    return result_vec;
}

// For "size_t* pop_data(float* const* const* output_data, size_t* num_output_samples, std::chrono::steady_clock::time_point wait_until)"
std::vector<size_t> popDataMultipleBlocking(
    anira::InferenceHandler& inf_handler,
    const std::vector<std::vector<std::vector<float>>>& output_data,
    std::vector<size_t>& num_output_samples,
    double wait_until_ms // milliseconds to wait from now
) {
    // Compute wait_until as steady_clock::time_point
    auto now = std::chrono::steady_clock::now();
    auto wait_until = now + std::chrono::milliseconds(static_cast<long long>(wait_until_ms));

    // Convert output_data [tensor][channel][sample] -> float* const* const*
    std::vector<std::vector<float*>> output_tensor_ptrs;
    output_tensor_ptrs.reserve(output_data.size());
    for (const auto& tensor : output_data) {
        std::vector<float*> channel_ptrs;
        channel_ptrs.reserve(tensor.size());
        for (const auto& channel : tensor) {
            channel_ptrs.push_back(const_cast<float*>(channel.data()));
        }
        output_tensor_ptrs.push_back(std::move(channel_ptrs));
    }

    // Top-level float** array
    std::vector<float**> output_data_ptrs;
    output_data_ptrs.reserve(output_tensor_ptrs.size());
    for (auto& tensor_ptrs : output_tensor_ptrs) {
        output_data_ptrs.push_back(tensor_ptrs.data());
    }

    // Call the actual C++ function
    size_t* results = inf_handler.pop_data(output_data_ptrs.data(), num_output_samples.data(), wait_until);

    // Convert result size_t* → std::vector<size_t>
    std::vector<size_t> result_vec(output_data.size());
    std::memcpy(result_vec.data(), results, output_data.size() * sizeof(size_t));

    return result_vec;
}


EMSCRIPTEN_BINDINGS(InferenceHandlerBindings) {
    class_<anira::InferenceHandler>("InferenceHandler")
    .smart_ptr<std::shared_ptr<anira::InferenceHandler>>("InferenceHandler")
        .constructor<anira::PrePostProcessor&, anira::InferenceConfig&>()
        .function("setInferenceBackend", &anira::InferenceHandler::set_inference_backend)
        .function("prepareWithAudioConfig", select_overload<void(anira::HostConfig)>(&anira::InferenceHandler::prepare))
        .function("prepareWithCustomLatency", select_overload<void(anira::HostConfig, unsigned int, size_t)>(&anira::InferenceHandler::prepare))
        .function("prepareWithLatencyVector", select_overload<void(anira::HostConfig, std::vector<unsigned int>)>(&anira::InferenceHandler::prepare))
        .function("processInPlace", &process_in_place)
        .function("processSeparateBuffers", &process_separate_buffers)
        .function("pushDataSingleTensor", &push_data_single_tensor)
        .function("pushDataMultipleTensors", &push_data_multi_tensor)
        .function("popDataSingleNonBlocking", &popDataSingleNonBlocking)
        .function("popDataSingleBlocking", &popDataSingleBlocking)
        .function("popDataMultipleNonBlocking", &popDataMultipleNonBlocking)
        .function("popDataMultipleBlocking", &popDataMultipleBlocking)
        .function("getLatency", &anira::InferenceHandler::get_latency)
        .function("getLatencyVector", &anira::InferenceHandler::get_latency_vector)
        .function("getAvailableSamples", &anira::InferenceHandler::get_available_samples)
        .function("setNonRealtime", &anira::InferenceHandler::set_non_realtime)
        .function("resetHandler", &anira::InferenceHandler::reset);
}
