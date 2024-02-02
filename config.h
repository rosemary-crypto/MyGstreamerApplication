#pragma once
#include <iostream>
#include <string>

namespace PintelGstreamerConfiguration {

    // Enum
    enum class STREAM_TYPE {
        RTSP_STREAM = 0,
        URI_UPLOAD_STREAM
    };

    enum class TRACKER_TYPE {
        COMMON_TRACKER,
        DEEP_SORT,
        DIRECTIONAL_SORT
    };

    enum class INFERENCE_MODELS_TYPE {
        YOLOV5_PHRD,
        YOLOV7,
        YOLOV8,
        PHRD_CLS2,
        PHRD_CLS4,
        PHRD_PM,
        ACCIDENT_DETECT_CSN,

    };

    INFERENCE_MODELS_TYPE inf_str2type(const std::string& str) {
        if (str == "pintelinference") {
            return INFERENCE_MODELS_TYPE::YOLOV5_PHRD;
        }
        else if (str == "YOLOV7_PHRD") {
            return INFERENCE_MODELS_TYPE::YOLOV7;
        }
        else if (str == "ACCIDENT_DETECTION") {
            return INFERENCE_MODELS_TYPE::ACCIDENT_DETECT_CSN;
        }
        else {

            std::cerr << "[Error]:: There is no such " << str << " inference model type. Default Inference model : YOLOV5_PHRD is initialized" << std::endl;
            return INFERENCE_MODELS_TYPE::YOLOV5_PHRD;
        }
    }

    const gchar* inf_type2str(INFERENCE_MODELS_TYPE type) {
        /*
        * This function accepts the enum type and return the tracker plugin name.
        */
        switch (type)
        {
        case INFERENCE_MODELS_TYPE::YOLOV5_PHRD:  return "pintelinference";
        case INFERENCE_MODELS_TYPE::YOLOV7:  return "YOLOV7_PHRD";
        case INFERENCE_MODELS_TYPE::ACCIDENT_DETECT_CSN:  return "ACCIDENT_DETECTION";

        default: return "YOLOV5_PHRD";
        }
    }

    const gchar* tracker_type2str(TRACKER_TYPE type) {
        /*
        * This function accepts the enum type and return the tracker plugin name.
        */
        switch (type)
        {
        case TRACKER_TYPE::COMMON_TRACKER:  return "commontracker";
        case TRACKER_TYPE::DEEP_SORT:  return "deepsort";
        case TRACKER_TYPE::DIRECTIONAL_SORT:  return "directional";

        default: return "commontracker";
        }
    }

    TRACKER_TYPE tracker_str2type(const std::string& str) {
        if (str == "commontracker") {
            return TRACKER_TYPE::COMMON_TRACKER;
        }
        else if (str == "deepsort") {
            return TRACKER_TYPE::DEEP_SORT;
        }
        else if (str == "directional") {
            return TRACKER_TYPE::DIRECTIONAL_SORT;
        }
        else {

            std::cerr << "[Error]:: There is no such " << str << " tracker type. Default tracker: commontracker is initialized" << std::endl;

            return TRACKER_TYPE::COMMON_TRACKER;
        }
    }



    //**********************Configurations *********************

    typedef struct {
        int stream_id;
        STREAM_TYPE type;
        int gpu_id;
        std::string uri;
    }StreamConfig;

    typedef struct {
        int tracker_id;
        int stream_id;
        TRACKER_TYPE tracker_type;
        int gpu_id;
        std::string config_uri;
        std::string save_log_url;
    } TrackerConfig;


    typedef struct {
        int inf_id;
        INFERENCE_MODELS_TYPE inf_model_type;
        int gpu_id;
        std::string config_uri;

    }InferenceConfig;

    struct ConfigData {
        std::vector<StreamConfig> stream;
        InferenceConfig inference;
        std::vector<TrackerConfig> tracker;
    };
};