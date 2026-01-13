#pragma once

#include "map_utils.h"
#include "map_layer.h"
#include "map_tilset.h"
#include "engine/camera.h"

namespace engine {
    class Renderer;
}

namespace tilemap {

    class TileMap {
    public:
        TileMap() = default;
        ~TileMap() = default;
        
        bool load(const engine::fs::path& mapFile);
        bool unload();

        const Vec2i& mapSize() const { return _mapSize; }
        const Vec2i& tileSize() const { return _tileSize; }

        void bake(engine::ResourceManager& resourceMgr);

        void draw(engine::Renderer& renderer, const engine::Camera& camera);

        const auto& collisionPoints() const { return _collisionPoints; }
        
        ImageLayer* getBkgroundLayer();
        TileLayer* getTileLayer();
        ObjectLayer* getObjectLayer();
        TileLayer* getDecorateLayer();

        template<typename T>
        std::optional<T> getObjectProperty(int objectId, const std::string& name) const;

        template<typename T>
        std::optional<T> getTileProperty(int x, int y, const std::string& name) const;

        template<typename T>
        std::optional<T> getTileProperty(int tileGid, const std::string& name) const;

        template<typename T>
        std::optional<T> getLayerProperty(int id, const std::string& name) const;

    private:
        bool load_mapdata(const json& json);

        bool load_layers(const json& json);
        
        bool load_tilesets(const json& json);

        bool load_one_tileset(const fs::path& filepath, int firstgid);

        void bakeTileLayer(engine::ResourceManager& resourceMgr, TileLayer& layer);
        void bakeImageLayer(engine::ResourceManager& resourceMgr, ImageLayer& layer);
        void bakeObjectLayer(engine::ResourceManager& resourceMgr, ObjectLayer& layer);

        TileSet* getTilesetOfTile(int tileGid) const;

        MapTile* getInfoOfTile(int tileGid) const;

        MapLayer* getLayer(int id) const;

        int getGidOfTile(int layerId, int x, int y) const;

        template<typename T>
        std::optional<T> getObjectProperty(int layerId, int objectId, const std::string& name) const;

        template<typename T>
        std::optional<T> getTileProperty(int layerId, int x, int y, const std::string& name) const;

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
    std::optional<T> TileMap::getObjectProperty(int objectId, const std::string& name) const
    {
        return getObjectProperty<T>((int)LayerId::Object, objectId, name);
    }

    template<typename T>
    std::optional<T> TileMap::getTileProperty(int x, int y, const std::string& name) const
    {
        return getTileProperty<T>((int)LayerId::TileMap, x, y, name);
    }

    template<typename T>
    std::optional<T> TileMap::getTileProperty(int tileGid, const std::string& name) const
    {
        auto maptile = getInfoOfTile(tileGid);
        if(!maptile) {
            return std::nullopt;
        }

        return maptile->properties.get<T>(name);
    }

    template<typename T>
    std::optional<T> TileMap::getTileProperty(int layerId, int x, int y, const std::string& name)  const {

        int gid = getGidOfTile(layerId, x, y);
        if(gid < 0)
        {
            return std::nullopt;
        }

        return getTileProperty<T>(gid, name);
    }

    template<typename T>
    std::optional<T> TileMap::getLayerProperty(int id, const std::string& name) const
    {
        auto layer = getLayer(id);
        if(!layer) {
            return std::nullopt;
        }

        return layer->properties.get<T>(name);
    }

    template<typename T>
    std::optional<T> TileMap::getObjectProperty(int layerId, int objectId, const std::string& name) const
    {
        auto layer = getLayer(layerId);
        if(!layer || layer->type != MapLayerType::ObjectLayer) {
            return std::nullopt;
        }

        auto objectLayer = dynamic_cast<ObjectLayer*>(layer);
        if(!objectLayer) {
            return std::nullopt;
        }

        auto it = objectLayer->objects.find(objectId);
        if(it == objectLayer->objects.end()) {
            return std::nullopt;
        }

        // get prop in object
        auto& object = it->second;
        auto iter = object.properties.find(name);
        if(iter == object.properties.end()) {
            if(object.gid > 0) {
                // get prop in tileset
                return getTileProperty<T>(object.gid, name);
            }

            return std::nullopt;
        }

        return iter->second.convert<T>();
    }
}