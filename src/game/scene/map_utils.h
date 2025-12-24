#pragma once


#include "engine/wrapper.h"
#include "engine/texture.h"
#include "engine/resource.h"
#include "utility/dynamic_struct.h"
#include "nlohmann/json.hpp"

namespace game {

    using namespace engine;

    using json = nlohmann::json;
    using Properties = utility::DynamicStruct<std::string>;

    enum class WalkType {
        Collision = 0,
        Walk = 1,
        Swim = 2,
    };

    enum class MapLayerType {
        ImageLayer,
        TileLayer,
        ObjectLayer,
        GroupLayer
    };

    struct MapDrawCall {
        std::vector<engine::Vertex> vertexies;
        engine::Texture* texture;
    };

    struct LoaderUtils {
        static void loadProperties(const json& json, Properties& properties);
    };


}