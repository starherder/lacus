#include "tile_map.h"

#include <memory>

namespace game {

    
bool TileMap::load(const fs::path& filepath)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        spdlog::error("tilemap open file {} failed.", filepath);
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

bool TileMap::load_mapdata(const json& json)
{
    LoaderUtils::loadProperties(json_data, _properties);

    version = json_data["version"].get<std::string>();
    tiledVersion = json_data["tiledversion"].get<std::string>();
    orientation = json_data["orientation"].get<std::string>();
    renderOrder = json_data["renderorder"].get<std::string>();
    type = json_data["type"].get<std::string>();

    infinite = json_data["infinite"].get<bool>();
    compressionLevel = json_data["compressionlevel"].get<int>();

    nextLayerId = json_data["nextlayerid"].get<int>();
    nextObjectId = json_data["nextobjectid"].get<int>();

    tileSize.x = json_data["tilesize"].get<int>();
    tileSize.y = json_data["tilesize"].get<int>();

    mapSize.x = json_data["width"].get<int>();
    mapSize.y = json_data["height"].get<int>();
 
    return true;
}

bool TileMap::load_layers(const json& json)
{
    for (auto layer_json: : json) {
        auto type = layer_json["type"].get<std::string>();

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
            if(layer_json.contains("layers") {
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
            _layers.push_back(layer);
        }
    }
    return true;
}
        
bool TileMap::load_tilesets(const json& json)
{
    return true;
}

}