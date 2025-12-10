#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "anira/InferenceConfig.h"
#include "anira/utils/HostConfig.h"
#include "anira/utils/RingBuffer.h"
#include "anira/utils/Buffer.h"

using namespace emscripten;

// ------ Vectors ----

template <typename T>
void bindVectorWithFactory(const std::string& name) {
    std::string factoryName = "make" + name;
    register_vector<T>(name.c_str());

    function(factoryName.c_str(), optional_override([](val jsArray) {
        std::vector<T> v = vecFromJSArray<T>(jsArray);
        return v;
    }));
}

EMSCRIPTEN_BINDINGS(VectorBindings) {
    bindVectorWithFactory<size_t>("VectorSizeT");
    bindVectorWithFactory<int64_t>("VectorInt64T");
    bindVectorWithFactory<float>("VectorFloat");
    bindVectorWithFactory<unsigned int>("VectorUnsignedInt");
    bindVectorWithFactory<std::vector<int64_t>>("VectorVectorInt64");
    bindVectorWithFactory<std::vector<float>>("VectorVectorFloat");
    bindVectorWithFactory<std::vector<std::vector<float>>>("VectorVectorVectorFloat");

    bindVectorWithFactory<anira::ModelData>("VectorModelData");
    bindVectorWithFactory<anira::TensorShape>("VectorTensorShape");
    bindVectorWithFactory<anira::TensorShapeList>("VectorTensorShapeList");
    // bindVectorWithFactory<anira::HostConfig>("vector<HostConfig>");
    bindVectorWithFactory<anira::RingBuffer>("VectorRingBuffer");
    bindVectorWithFactory<anira::BufferF>("VectorBufferF");
}
