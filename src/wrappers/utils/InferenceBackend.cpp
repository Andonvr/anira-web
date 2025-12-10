#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "anira/utils/InferenceBackend.h"

using namespace emscripten;

// ------ InferenceBackend ----

EMSCRIPTEN_BINDINGS(InferenceBackendBindings) {
    emscripten::enum_<anira::InferenceBackend>("InferenceBackend")
        .value("ONNX", anira::InferenceBackend::ONNX)
        .value("CUSTOM", anira::InferenceBackend::CUSTOM);
}