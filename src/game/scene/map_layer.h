#pragma once

#include "map_utils.h"


namespace game {

    enum class MapLayerType {
        ImageLayer,
        TileLayer,
        ObjectLayer,
        GroupLayer
    };


    struct MapObject {
        int id = -1;
        int gid = -1;
        std::string name;
        std::string type;
        bool visible = true;
        int rotation = 0;
        Vec2i pos;
        Vec2i size;

        Properties properties;
        
        bool load(const json& json);
    };

    struct MapLayer 
    {
        int id = -1;
        std::string name;
        bool visible = true;;

        MapLayerType type;
        int opacity = 0;

        Color tint_color;

        Vec2i pos;

        Properties properties;

        virtual bool load(const json& json);
    };


    struct ImageLayer : public MapLayer 
    {
        std::string image_file;   

        Vec2i offset;
        Vec2i image_size;

        bool repeat_x = false;
        bool repeat_y = false;

        bool load(const json& json) override;
    };

    struct TileLayer : public MapLayer {
        int width = 0;
        int height = 0;

        std::string encodeing;
        std::string compression;
        std::vector<int> data;

        bool load(const json& json) override;
    };

    struct ObjectLayer : public MapLayer {

        std::string draw_order;
        std::map<int, MapObject> objects;

        bool load(const json& json) override;
    };
    
    struct GroupLayer : public MapLayer {
        bool load(const json& json) override;
    };


}