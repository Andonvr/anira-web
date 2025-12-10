#ifndef ANIRA_CNNCONFIG_H
#define ANIRA_CNNCONFIG_H

#include <anira/anira.h>

static std::vector<anira::ModelData> model_data_cnn_config = {
    {STEERABLENAFX_MODELS_PATH_PYTORCH + std::string("/model_0/steerable-nafx-libtorch-dynamic.onnx"), anira::InferenceBackend::ONNX},
    {STEERABLENAFX_MODELS_PATH_PYTORCH + std::string("/model_0/steerable-nafx-libtorch-dynamic.onnx"), anira::InferenceBackend::CUSTOM},
};

static std::vector<anira::TensorShape> tensor_shape_cnn_config = {
    {{{1, 1, 15380}}, {{1, 1, 2048}}},
};

static anira::ProcessingSpec processing_spec_cnn_config = {
        {1}, // preprocess_input_channels
        {1}, // postprocess_output_channels
        {2048}, // preprocess_input_size
        {2048} // postprocess_output_size
};

static anira::InferenceConfig cnn_config (
        model_data_cnn_config,
        tensor_shape_cnn_config,
        processing_spec_cnn_config,
        42.66f,
        2
);


#endif //ANIRA_CNNCONFIG_H
