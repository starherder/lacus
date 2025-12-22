#include "tile_map.h"

#include "engine/render.h"
#include "magic_enum/magic_enum.h"

namespace game {


    
bool TileMap::load(const engine::fs::path& filepath)
{
    spdlog::info("load tilemap: {}", filepath.string());

    std::ifstream file(filepath);
    if (!file.is_open()) 
    {
        spdlog::error("tilemap open file {} failed.", filepath.string());
        return false;
    }

    nlohmann::json json_data;

    try 
    {
        file >> json_data;
    } 
    catch (const nlohmann::json::parse_error& e) 
    {
        spdlog::error("parse json failed: {}", e.what());
        return false;
    }

    _mapPath = filepath.parent_path();

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

void TileMap::draw(engine::Renderer& renderer)
{
    for (const auto& dc : _drawCalls)
    {
        renderer.drawGeometry(dc->texture, dc->vertexies.data(), (int)(dc->vertexies.size()), nullptr, 0);
    }
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
        auto id = layer_json.value("id", 0);
        auto type = layer_json.value("type", "");

        spdlog::info("load layer: {}, type: {}", id, type);

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

bool TileMap::load_one_tileset(const fs::path& filepath, int firstgid)
{
    spdlog::info("load tileset: {}, firstgid: {}", filepath.string(), firstgid);

    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::error("tileset open file {} failed.", filepath.string());
        return false;
    }

    nlohmann::json json_data;
    try {
        file >> json_data;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("tilset parse json failed: {}", e.what());
        return false;
    }

    auto ts = std::make_unique<TileSet>();
    ts->firstgid = firstgid;
    ts->load(json_data);

    _tilesets.push_back(std::move(ts));
    return true;
}

bool TileMap::load_tilesets(const json& jstilesets)
{
    for(auto& jsitem : jstilesets) 
    {
        auto firstgid = jsitem.value("firstgid", 1);
        auto source = jsitem.value("source", "");

        auto tilesetfile = _mapPath / source;
        load_one_tileset(tilesetfile, firstgid);
    }
    return true;
}

void TileMap::bakeGeometry(engine::ResourceManager& resourceMgr)
{
    for(auto& layer : _layers) 
    {
        if(layer && layer->type==MapLayerType::TileLayer) 
        {
            auto& tileLayer = dynamic_cast<TileLayer&>(*layer);
            bakeTileLayer(resourceMgr, tileLayer);
        }
        else if (layer && layer->type == MapLayerType::ImageLayer)
        {
            auto& imageLayer = dynamic_cast<ImageLayer&>(*layer);
            bakeImageLayer(resourceMgr, imageLayer);
        }
        else if (layer && layer->type == MapLayerType::ObjectLayer)
        {
            auto& objectLayer = dynamic_cast<ObjectLayer&>(*layer);
            bakeObjectLayer(resourceMgr, objectLayer);
        }
        else if (layer && layer->type == MapLayerType::GroupLayer)
        {
            spdlog::info("layer: {} type <GroupLayer> do nothing...", layer->name);
        }
        else
        {
            spdlog::warn("layer: {} type {} NOT support.", layer->name, magic_enum::enum_name(layer->type));
        }
    }
}

void TileMap::bakeTileLayer(engine::ResourceManager& resourceMgr, TileLayer& layer)
{
    for (auto& tileset : _tilesets)
    {
        if (tileset)
        {
            auto mapRelatePath = _mapPath.lexically_relative(resourceMgr.resPath());
            auto imagePath = mapRelatePath / tileset->imageFile;
            tileset->texture = resourceMgr.textureManager().get(HashString(imagePath.string().c_str()));
            if (!tileset->texture)
            {
                spdlog::error("load texture {} failed.", imagePath.string());
            }
        }
    }

    auto vertColour = (SDL_FColor)layer.tint_color;
    for(auto& ptileset : _tilesets)
    {
        const auto& tileset = *ptileset;
        const auto& tileData = layer.data;

        float margin = (float)tileset.margin;
        float space = (float)tileset.spacing;

        auto textTileSize = tileset.tileSize;
        auto textSize = tileset.imageSize;
        auto textTileColumn = tileset.columns;

        float uNorm = (float)(textTileSize.x) / textSize.x;
        float vNorm = (float)(textTileSize.y) / textSize.y;

        std::vector<engine::Vertex> verts;
        for (int y = 0; y < mapSize.y; ++y)
        {
            for (int x = 0u; x < mapSize.x; ++x)
            {
                const auto idx = y * mapSize.x + x;
                if (idx >= tileData.size())
                {
                    continue;
                }

                const auto tileId = tileData[idx];
                if (tileId >= tileset.firstgid && tileId < (tileset.firstgid + tileset.tileCount))
                {
                    //tex coords
                    auto idIndex = (tileId - tileset.firstgid);

                    float u = (float)(idIndex % textTileColumn);
                    float v = (float)(idIndex / textTileColumn);
                    
                    u = (u * (textTileSize.x + space) + margin ) / textSize.x;
                    v = (v * (textTileSize.y + space) + margin ) / textSize.y;

                    //vert pos
                    const float tilePosX = (float)(x) * tileSize.x;
                    const float tilePosY = (float)(y) * tileSize.y;

                    //push back to vert array
                    engine::Vertex vert = { { tilePosX, tilePosY }, vertColour, {u, v} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX + tileSize.x, tilePosY }, vertColour, {u + uNorm, v} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX, tilePosY + tileSize.y}, vertColour, {u, v + vNorm} };
                    verts.emplace_back(vert);

                    vert = { { tilePosX, tilePosY + tileSize.y}, vertColour, {u, v + vNorm} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX + tileSize.x, tilePosY }, vertColour, {u + uNorm, v} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX + tileSize.x, tilePosY + tileSize.y }, vertColour, {u + uNorm, v + vNorm} };
                    verts.emplace_back(vert);
                }
            }
        }

        if (!verts.empty())
        {
            auto drawcall = std::make_unique<MapDrawCall>();
            drawcall->texture = tileset.texture;
            drawcall->vertexies.swap(verts);

            _drawCalls.push_back(std::move(drawcall));
        }
    }
}

void TileMap::bakeImageLayer(engine::ResourceManager& resourceMgr, ImageLayer& layer)
{
    if (!layer.visible)
    {
        return;
    }

    auto mapRelatePath = _mapPath.lexically_relative(resourceMgr.resPath());
    auto imagePath = mapRelatePath / layer.image_file;
    auto texture = resourceMgr.textureManager().get(HashString(imagePath.string().c_str()));
    if (!texture)
    {
        spdlog::error("bake image layer: load image failed, image = {}", imagePath.string());
        return;
    }

    Vec2 pos = layer.pos + layer.offset;
    Vec2 size = layer.image_size;

    Vec2 lt = pos;
    Vec2 lb = pos + Vec2{ 0, size.y };
    Vec2 rt = pos + Vec2{ size.x, 0 };
    Vec2 rb = pos + size;

    auto vertColour = (SDL_FColor)layer.tint_color;

    engine::Vertex vert;
    std::vector<engine::Vertex> verts;

    vert = { { lt.x, lt.y }, vertColour, {0, 0} };
    verts.emplace_back(vert);
    vert = { { rt.x, rt.y }, vertColour, {1, 0} };
    verts.emplace_back(vert);
    vert = { { rb.x, rb.y }, vertColour, {1, 1} };
    verts.emplace_back(vert);

    vert = { { lt.x, lt.y }, vertColour, {0, 0} };
    verts.emplace_back(vert);
    vert = { { rb.x, rb.y }, vertColour, {1, 1} };
    verts.emplace_back(vert);
    vert = { { lb.x, lb.y }, vertColour, {0, 1} };
    verts.emplace_back(vert);

    auto drawcall = std::make_unique<MapDrawCall>();
    drawcall->texture = texture;
    drawcall->vertexies.swap(verts);

    _drawCalls.push_back(std::move(drawcall));
}

void TileMap::bakeObjectLayer(engine::ResourceManager& resourceMgr, ObjectLayer& layer)
{

}


}