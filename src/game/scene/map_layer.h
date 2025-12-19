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

    enum class MapLayerType {
        Image,
        Tile,
        Object
    };

    struct LoaderUtils {
        static void loadProperties(const json& json, Properties& properties);
    };


    struct MapObject {
        int id;
        int gid;
        std::string name;
        std::string type;
        bool visible;
        int rotation;
        Vec2i pos;
        Vec2i size;

        Properties properties;
        
        bool load(const json& json);
    }

    struct MapLayer 
    {
        int id;
        std::string name;
        bool visible;

        MapLayerType type;
        int opacity;

        Vec2i pos;

        Properties properties;

        //std::vector<MapDrawCall> drawInfos;

        virtual bool load(const json& json);
    };


    struct ImageLayer : public MapLayer 
    {
        bool load(const json& json) override;
        
        std::string image_file;   

        Vec2i offset;
        Vec2i image_size;

        bool repeat_x;
        bool repeat_y;
    };

    struct TileLayer : public MapLayer {
        bool load(const json& json) override;

        int width;
        int height;

        std::string encodeing;
        std::string compression;
        std::vector<int> data;
    };

    struct ObjectLayer : public MapLayer {
        bool load(const json& json) override;

        std::string draw_order;

        std::vector<MapObject> objects;
    };
    
    struct GroupLayer : public MapLayer {
        bool load(const json& json) override;
    };


}