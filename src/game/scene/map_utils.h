#pragma once


#include "engine/wrapper.h"
#include "engine/texture.h"
#include "utility/dynamic_struct.h"
#include "nlohmann/json.hpp"

namespace game {

    using namespace engine;

    using json = nlohmann::json;
    using Properties = utility::DynamicStruct<std::string>;

    struct MapDrawCall {
        std::vector<engine::Vertex> vertices;
        engine::Texture* texture;
    };

    struct LoaderUtils {
        static void loadProperties(const json& json, Properties& properties);
    };


}