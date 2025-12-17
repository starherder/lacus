#include "TmxMap.h"
#include <tmxlite/Map.hpp>

#if 0

namespace tmx {

using namespace engine;

TmxMapManager::TmxMapManager(engine::ResourceManager& resourceMgr) 
    : _resourceManager(resourceMgr)
{
}

TmxMap* TmxMapManager::load(const std::string& name, const std::string& filepath)
{
    auto tmxFilePath = fs::current_path() / filepath;
    if(!fs::exists(tmxFilePath))
    {
        spdlog::error("map {}, path({}) NOT exist.", name, filepath);
        return nullptr;
    }

    auto tmxDir = tmxFilePath.parent_path();
    
    tmx::Map map;
    if(!map.load(tmxFilePath.string()))
    {
        spdlog::error("load file {} failed.", tmxFilePath.string());
        return nullptr;
    }

    const auto& tileSets = map.getTilesets();
    for (const auto& ts : tileSets)
    {
        auto& tsName = ts.getName();
        auto& imageFile = ts.getImagePath();
        auto imagePath = tmxDir / imageFile;

        auto texture = _resourceManager.textureManager().get(tsName, imagePath.string());
        if (!texture) {
            spdlog::error("load texture({}) file({}) failed", tsName, imagePath.string());
        }
    }

    const auto& mapLayers = map.getLayers();
    for(auto& layer : mapLayers) 
    {
        if (layer && layer->getType() == tmx::Layer::Type::Tile) {
            loadTileLayer(map, layer);
        }
    }

    return true;
}

TmxMap* TmxMapManager::get(const std::string& name, const std::string& path)
{
    auto it = _maps.find(name);
    if (it != _maps.end())
    {
        return it->second.get();
    }

    if (path.empty())
    {
        return nullptr;
    }

    return load(name, path);
}



void TmxMapManager::loadTileLayer(tmx::Map& map, const LayerPtr& layer)
{

}

bool TmxMapManager::unload(const std::string& name)
{
    return true;
}

    
}

#endif