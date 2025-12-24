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

        const Vec2i& mapSize() { return _mapSize; }
        const Vec2i& tileSize() { return _tileSize; }

        void bake(engine::ResourceManager& resourceMgr);

        void draw(engine::Renderer& renderer);

        template<typename T>
        std::pair<bool, T> getObjectProperty(int layerId, int objectId, const std::string& name);

        template<typename T>
        std::pair<bool, T> getTileProperty(int layerId, int x, int y, const std::string& name);

        template<typename T>
        std::pair<bool, T> getTileProperty(int tileGid, const std::string& name);

        template<typename T>
        std::pair<bool, T> getLayerProperty(int id, const std::string& name);

        const auto& collisionPoints() const { return _collisionPoints; }

    private:
        bool load_mapdata(const json& json);

        bool load_layers(const json& json);
        
        bool load_tilesets(const json& json);
        bool load_one_tileset(const fs::path& filepath, int firstgid);

        void bakeTileLayer(engine::ResourceManager& resourceMgr, TileLayer& layer);
        void bakeImageLayer(engine::ResourceManager& resourceMgr, ImageLayer& layer);
        void bakeObjectLayer(engine::ResourceManager& resourceMgr, ObjectLayer& layer);

        TileSet* getTilesetOfTile(int tileGid);

        MapTile* getInfoOfTile(int tileGid);

        int getGidOfTile(int layerId, int x, int y);

        MapLayer* getLayer(int id);

    private:
        fs::path _resPath;
        fs::path _mapPath;

        Properties _properties;
        
        std::map<int, std::shared_ptr<MapLayer>> _layers;
        std::map<int, std::shared_ptr<TileSet>> _tilesets;
        std::multimap<int, std::shared_ptr<MapDrawCall>> _drawCalls;

        std::vector<Vec2i> _collisionPoints;

        std::string _version;
        std::string _tiledVersion;
        std::string _orientation;
        std::string _renderOrder;
        std::string _type;

        bool _infinite;
        int _compressionLevel;

        int _nextLayerId;
        int _nextObjectId;

        Vec2i _tileSize;
        Vec2i _mapSize;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    std::pair<bool, T> TileMap::getTileProperty(int tileGid, const std::string& name)
    {
        auto maptile = getInfoOfTile(tileGid);
        if(!maptile) {
            return {false, T{}};
        }

        return maptile->properties.get<T>(name);
    }

    template<typename T>
    std::pair<bool, T> TileMap::getTileProperty(int layerId, int x, int y, const std::string& name) {

        int gid = getGidOfTile(layerId, x, y);
        if(gid < 0)
        {
            return {false, T{}};
        }

        return getTileProperty<T>(gid, name);
    }

    template<typename T>
    std::pair<bool, T> TileMap::getLayerProperty(int id, const std::string& name)
    {
        auto layer = getLayer(id);
        if(!layer) {
            return {false, T{}};
        }

        return layer->properties.get<T>(name);
    }

    template<typename T>
    std::pair<bool, T> TileMap::getObjectProperty(int layerId, int objectId, const std::string& name)
    {
        auto layer = getLayer(layerId);
        if(!layer || layer->type != MapLayerType::ObjectLayer) {
            return {false, T{}};
        }

        auto objectLayer = dynamic_cast<ObjectLayer*>(layer);
        if(!objectLayer) {
            return {false, T{}};
        }

        auto it = objectLayer->objects.find(objectId);
        if(it == objectLayer->objects.end()) {
            return {false, T{}};
        }

        // get prop in object
        auto& object = it->second;
        auto it = object.properties.find(name);
        if(it == object.properties.end()) {
            if(object.gid > 0) {
                // get prop in tileset
                return getTileProperty(object.gid, name);
            }

            return {false, T{}};
        }

        return {true, (T)it->second};
    }
}