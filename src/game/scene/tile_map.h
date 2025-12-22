#pragma once

#include "map_utils.h"
#include "map_layer.h"
#include "map_tilset.h"

namespace game {

    class TileMap {
    public:
        TileMap() = default;
        ~TileMap() = default;
        
        bool load(const fs::path& filepath);
        bool unload();

    private:
        bool load_mapdata(const json& json);

        bool load_layers(const json& json);
        
        bool load_tilesets(const json& json);

    private:
        Properties _properties;
        std::vector<std::unique_ptr<MapLayer>> _layers;

        std::string version;
        std::string tiledVersion;
        std::string orientation;
        std::string renderOrder;
        std::string type;

        bool infinite;
        int compressionLevel;

        int nextLayerId;
        int nextObjectId;

        Vec2i tileSize;
        Vec2i mapSize;
    };
}