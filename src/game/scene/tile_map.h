#pragma once

#include "map_utils.h"
#include "map_layer.h"
#include "map_tilset.h"

namespace engine {
    class Renderer;
}

namespace game {

    class TileMap {
    public:
        TileMap() = default;
        ~TileMap() = default;
        
        bool load(const engine::fs::path& mapFile);
        bool unload();

        void bakeGeometry(engine::ResourceManager& resourceMgr);
        void draw(engine::Renderer& renderer);

    private:
        bool load_mapdata(const json& json);

        bool load_layers(const json& json);
        
        bool load_tilesets(const json& json);
        bool load_one_tileset(const fs::path& filepath, int firstgid);

        void bakeTileLayer(TileLayer& layer);

    private:
        fs::path _resPath;
        fs::path _mapPath;

        Properties _properties;
        
        std::vector<std::unique_ptr<MapLayer>> _layers;

        std::vector<std::unique_ptr<TileSet>> _tilesets;

        std::vector<std::unique_ptr<MapDrawCall>> _drawCalls;

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