#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "anira/InferenceConfig.h"
#include <anira/utils/InferenceBackend.h>

using namespace emscripten;

// ------ ModelData ----

std::shared_ptr<anira::ModelData> makeModelData(uintptr_t dataPtr,
                                                size_t size,
                                                anira::InferenceBackend backend,
                                                const std::string& name,
                                                bool flag) {
    return std::make_shared<anira::ModelData>((void*)dataPtr, size, backend, name, flag);
}


std::shared_ptr<anira::ModelData> createModelDataFromBuffer(val arrayBuffer) {
    size_t length = arrayBuffer["byteLength"].as<size_t>();

    void* data = std::malloc(length);
    if (!data) {
        throw std::bad_alloc();
    }

    emscripten::val uint8Array = emscripten::val::global("Uint8Array").new_(arrayBuffer);
    emscripten::val wasmView = emscripten::val(emscripten::typed_memory_view(length, static_cast<uint8_t*>(data)));
    wasmView.call<void>("set", uint8Array);

    return std::make_shared<anira::ModelData>(data, length, anira::InferenceBackend::ONNX, "", true);
}


EMSCRIPTEN_BINDINGS(ModelDataBindings) {
    class_<anira::ModelData>("ModelData")
    .smart_ptr<std::shared_ptr<anira::ModelData>>("ModelData")
        .constructor(&makeModelData, allow_raw_pointers())
        .constructor<const std::string&, anira::InferenceBackend, const std::string&, bool>()
        .function("equals", &anira::ModelData::operator==)
        .function("notEquals", &anira::ModelData::operator!=)
        // Getter and Setter needed because emscripten does not support void* directly
        .function("getDataPtr", optional_override([](anira::ModelData& self) {
            return reinterpret_cast<uintptr_t>(self.m_data);
        }))
        .function("setDataPtr", optional_override([](anira::ModelData& self, uintptr_t ptrVal) {
            self.m_data = reinterpret_cast<void*>(ptrVal);
        }))
        .property("size", &anira::ModelData::m_size)
        .property("backend", &anira::ModelData::m_backend)
        .property("modelFunction", &anira::ModelData::m_model_function)
        .property("isBinary", &anira::ModelData::m_is_binary)
        
        .class_function("createFromBuffer", &createModelDataFromBuffer);
}


// ------ TensorShape, TensorShapeList ----

std::vector<std::vector<int64_t>> createTensorShapeList(const emscripten::val& jsArray) {
    std::vector<std::vector<int64_t>> result;
    unsigned length = jsArray["length"].as<unsigned>();
    for (unsigned i = 0; i < length; i++) {
        emscripten::val innerArray = jsArray[i];
        unsigned innerLength = innerArray["length"].as<unsigned>();
        std::vector<int64_t> innerVec;
        innerVec.reserve(innerLength);
        for (unsigned j = 0; j < innerLength; j++) {
            innerVec.push_back(innerArray[j].as<int64_t>());
        }
        result.push_back(std::move(innerVec));
    }
    return result;
}

EMSCRIPTEN_BINDINGS(TensorShapeBindings) {
    class_<anira::TensorShape>("TensorShape")
    .smart_ptr<std::shared_ptr<anira::TensorShape>>("TensorShape")
        .constructor<anira::TensorShapeList, anira::TensorShapeList>() // Universal constructor
        .constructor<anira::TensorShapeList, anira::TensorShapeList, anira::InferenceBackend>() // Backend-specific constructor
        .function("isUniversal", &anira::TensorShape::is_universal)
        .function("equals", &anira::TensorShape::operator==)
        .function("notEquals", &anira::TensorShape::operator!=)
        .property("tensorInputShape", &anira::TensorShape::m_tensor_input_shape)
        .property("tensorOutputShape", &anira::TensorShape::m_tensor_output_shape)
        .property("backend", &anira::TensorShape::m_backend)
        .property("universal", &anira::TensorShape::m_universal)
        .class_function("createTensorShapeList", &createTensorShapeList);
}

// ------ ProcessingSpec ----

EMSCRIPTEN_BINDINGS(ProcessingSpecBindings) {
    class_<anira::ProcessingSpec>("ProcessingSpec")
    .smart_ptr<std::shared_ptr<anira::ProcessingSpec>>("ProcessingSpec")
        .constructor<>()
        .constructor<std::vector<size_t>, std::vector<size_t>>()
        .constructor<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>>()
        .constructor<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>, std::vector<size_t>>()
        .function("equals", &anira::ProcessingSpec::operator==)
        .function("notEquals", &anira::ProcessingSpec::operator!=)
        .property("preprocessInputChannels", &anira::ProcessingSpec::m_preprocess_input_channels)
        .property("postprocessOutputChannels", &anira::ProcessingSpec::m_postprocess_output_channels)
        .property("preprocessInputSize", &anira::ProcessingSpec::m_preprocess_input_size)
        .property("postprocessOutputSize", &anira::ProcessingSpec::m_postprocess_output_size)
        .property("internalModelLatency", &anira::ProcessingSpec::m_internal_model_latency)
        .property("tensorInputSize", &anira::ProcessingSpec::m_tensor_input_size)
        .property("tensorOutputSize", &anira::ProcessingSpec::m_tensor_output_size);
}

// ------ InferenceConfig ----

EMSCRIPTEN_BINDINGS(InferenceConfigBindings) {
    class_<anira::InferenceConfig>("InferenceConfig")
    .smart_ptr<std::shared_ptr<anira::InferenceConfig>>("InferenceConfig")
        .constructor<>()
        .constructor<std::vector<anira::ModelData>, std::vector<anira::TensorShape>, anira::ProcessingSpec, float>()
        .constructor<std::vector<anira::ModelData>, std::vector<anira::TensorShape>, anira::ProcessingSpec, float, unsigned int>()
        .constructor<std::vector<anira::ModelData>, std::vector<anira::TensorShape>, anira::ProcessingSpec, float, unsigned int, bool, float, unsigned int>()
        .constructor<std::vector<anira::ModelData>, std::vector<anira::TensorShape>, float, unsigned int, bool, float, unsigned int>()
        .function("getModelPath", &anira::InferenceConfig::get_model_path)
        .function("getModelData", &anira::InferenceConfig::get_model_data, allow_raw_pointers())
        .function("getModelFunction", &anira::InferenceConfig::get_model_function)
        .function("isModelBinary", &anira::InferenceConfig::is_model_binary)
        .function("getTensorInputShapeUniversal", select_overload<const anira::TensorShapeList&() const>(&anira::InferenceConfig::get_tensor_input_shape))
        .function("getTensorOutputShapeUniversal", select_overload<const anira::TensorShapeList&() const>(&anira::InferenceConfig::get_tensor_output_shape))
        .function("getTensorInputShapeBackend", select_overload<const anira::TensorShapeList&(anira::InferenceBackend) const>(&anira::InferenceConfig::get_tensor_input_shape))
        .function("getTensorOutputShapeBackend", select_overload<const anira::TensorShapeList&(anira::InferenceBackend) const>(&anira::InferenceConfig::get_tensor_output_shape))
        .function("getTensorInputSize", &anira::InferenceConfig::get_tensor_input_size)
        .function("getTensorOutputSize", &anira::InferenceConfig::get_tensor_output_size)
        .function("getPreprocessInputChannels", &anira::InferenceConfig::get_preprocess_input_channels)
        .function("getPostprocessOutputChannels", &anira::InferenceConfig::get_postprocess_output_channels)
        .function("getPreprocessInputSize", &anira::InferenceConfig::get_preprocess_input_size)
        .function("getPostprocessOutputSize", &anira::InferenceConfig::get_postprocess_output_size)
        .function("getInternalModelLatency", &anira::InferenceConfig::get_internal_model_latency)
        .function("setTensorInputShapeUniversal", select_overload<void(const anira::TensorShapeList&)>(&anira::InferenceConfig::set_tensor_input_shape))
        .function("setTensorOutputShapeUniversal", select_overload<void(const anira::TensorShapeList&)>(&anira::InferenceConfig::set_tensor_output_shape))
        .function("setTensorInputShapeBackend", select_overload<void(const anira::TensorShapeList&, anira::InferenceBackend)>(&anira::InferenceConfig::set_tensor_input_shape))
        .function("setTensorOutputShapeBackend", select_overload<void(const anira::TensorShapeList&, anira::InferenceBackend)>(&anira::InferenceConfig::set_tensor_output_shape))
        .function("setPreprocessInputChannels", &anira::InferenceConfig::set_preprocess_input_channels)
        .function("setPreprocessOutputChannels", &anira::InferenceConfig::set_preprocess_output_channels)
        .function("setPreprocessInputSize", &anira::InferenceConfig::set_preprocess_input_size)
        .function("setPostprocessOutputSize", &anira::InferenceConfig::set_postprocess_output_size)
        .function("setInternalModelLatency", &anira::InferenceConfig::set_internal_model_latency)
        .function("setModelPath", &anira::InferenceConfig::set_model_path)
        .function("getTensorShape", &anira::InferenceConfig::get_tensor_shape, allow_raw_pointers())
        .function("clearProcessingSpec", &anira::InferenceConfig::clear_processing_spec)
        .function("updateProcessingSpec", &anira::InferenceConfig::update_processing_spec)
        .property("modelData", &anira::InferenceConfig::m_model_data)
        .property("tensorShape", &anira::InferenceConfig::m_tensor_shape)
        .property("processingSpec", &anira::InferenceConfig::m_processing_spec)
        .property("maxInferenceTime", &anira::InferenceConfig::m_max_inference_time)
        .property("warmUp", &anira::InferenceConfig::m_warm_up)
        .property("sessionExclusiveProcessor", &anira::InferenceConfig::m_session_exclusive_processor)
        .property("blockingRatio", &anira::InferenceConfig::m_blocking_ratio)
        .property("numParallelProcessors", &anira::InferenceConfig::m_num_parallel_processors);
}
