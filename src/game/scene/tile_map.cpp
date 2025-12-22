#include "tile_map.h"

#include <memory>

namespace game {

    
bool TileMap::load(const fs::path& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::error("tilemap open file {} failed.", filepath.string());
        return false;
    }

    nlohmann::json json_data;
    try {
        file >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("parse json failed: {}", e.what());
        return false;
    }

    load_mapdata(json_data);

    if(json_data.contains("layers"))
    {
        auto& layers = json_data["layers"];
        load_layers(layers);
    }

    if(json_data.contains("tilesets"))
    {
        auto& tilesets = json_data["tilesets"];
        load_tilesets(tilesets);
    }

    return true;
}

bool TileMap::unload()
{
    return true;
}

bool TileMap::load_mapdata(const json& json_data)
{
    LoaderUtils::loadProperties(json_data, _properties);

    version = json_data.value("version", "");
    tiledVersion = json_data.value("tiledversion", "");
    orientation = json_data.value("orientation", "");
    renderOrder = json_data.value("renderorder", "");
    type = json_data.value("type", "");

    infinite = json_data.value("infinite", false);
    compressionLevel = json_data.value("compressionlevel", 0);

    nextLayerId = json_data.value("nextlayerid", 0);
    nextObjectId = json_data.value("nextobjectid", 0);

    tileSize.x = json_data.value("tilewidth", 0);
    tileSize.y = json_data.value("tileheight", 0);

    mapSize.x = json_data.value("width", 0);
    mapSize.y = json_data.value("height", 0);
 
    return true;
}

bool TileMap::load_layers(const json& json)
{
    for (auto layer_json : json) {
        auto type = layer_json.value("type", "");

        std::unique_ptr<MapLayer> layer = nullptr;

        if(type=="tilelayer")
        {
            layer = std::make_unique<TileLayer>();
        }
        else if(type=="objectgroup")
        {
            layer = std::make_unique<ObjectLayer>();
        }
        else if(type=="imagelayer")
        {
            layer = std::make_unique<ImageLayer>();
        }
        else if(type=="group")
        {
            layer = std::make_unique<GroupLayer>();
            if(layer_json.contains("layers")) {
                auto& layers = layer_json["layers"];
                load_layers(layers);
            }
        }
        else 
        {
            spdlog::error("layer type: {} NOT support yet.", type);
        }

        if(layer && layer->load(layer_json))
        {
            _layers.push_back(std::move(layer));
        }
    }
    return true;
}
        
bool TileMap::load_tilesets(const json& json)
{
    for(auto& ts : json) {
        auto firstgid = ts.value("firstgid", 1);
        TileSet ts;
    
    }
    return true;
}

}