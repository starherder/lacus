
#include "TmxLayer.h"

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include <iostream>
#include <array>
#include <cassert>

#include "engine/texture.h"
#include "engine/render.h"

namespace tmx {

MapLayer::MapLayer()
{
}

bool MapLayer::create(const tmx::Map& map, std::uint32_t layerIndex, const std::vector<engine::Texture*>& textures)
{
    const auto& layers = map.getLayers();
    assert(layers[layerIndex]->getType() == tmx::Layer::Type::Tile);
    
    const auto& layer = layers[layerIndex]->getLayerAs<tmx::TileLayer>();
    const auto mapSize = map.getTileCount();
    const auto mapTileSize = map.getTileSize();
    const auto& tileSets = map.getTilesets();

    if (tileSets.size() != textures.size())
    {
        spdlog::error("tilesets.size != textures.size");
        return false;
    }

    const auto tintColour = layer.getTintColour();
    const engine::Color iColour =
    {
        tintColour.r,
        tintColour.g,
        tintColour.b,
        tintColour.a
    };

    auto vertColour = engine::ColorUtils::to_fcolor(iColour);

    for (auto i = 0u; i < tileSets.size(); ++i)
    {
        //check tile ID to see if it falls within the current tile set
        const auto& ts = tileSets[i];
        const auto& tileIDs = layer.getTiles();

        const auto texSize = textures[i]->size();
        const auto tileCountX = (int)(texSize.x / mapTileSize.x);
        const auto tileCountY = (int)(texSize.y / mapTileSize.y);

        const float uNorm = static_cast<float>(mapTileSize.x) / texSize.x;
        const float vNorm = static_cast<float>(mapTileSize.y) / texSize.y;

        std::vector<engine::Vertex> verts;
        for (auto y = 0u; y < mapSize.y; ++y)
        {
            for (auto x = 0u; x < mapSize.x; ++x)
            {
                const auto idx = y * mapSize.x + x;
                if (idx < tileIDs.size() && tileIDs[idx].ID >= ts.getFirstGID()
                    && tileIDs[idx].ID < (ts.getFirstGID() + ts.getTileCount()))
                {
                    //tex coords
                    auto idIndex = (tileIDs[idx].ID - ts.getFirstGID());

                    float u = static_cast<float>(idIndex % tileCountX);
                    float v = static_cast<float>(idIndex / tileCountX);
                    u *= mapTileSize.x; //TODO we should be using the tile set size, as this may be different from the map's grid size
                    v *= mapTileSize.y;

                    //normalise the UV
                    u /= textures[i]->size().x;
                    v /= textures[i]->size().y;

                    //vert pos
                    const float tilePosX = static_cast<float>(x) * mapTileSize.x;
                    const float tilePosY = (static_cast<float>(y) * mapTileSize.y);

                    //push back to vert array
                    engine::Vertex vert = { { tilePosX, tilePosY }, vertColour, {u, v} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX + mapTileSize.x, tilePosY }, vertColour, {u + uNorm, v} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX, tilePosY + mapTileSize.y}, vertColour, {u, v + vNorm} };
                    verts.emplace_back(vert);
                    
                    vert = { { tilePosX, tilePosY +mapTileSize.y}, vertColour, {u, v + vNorm} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX + mapTileSize.x, tilePosY }, vertColour, {u + uNorm, v} };
                    verts.emplace_back(vert);
                    vert = { { tilePosX + mapTileSize.x, tilePosY + mapTileSize.y }, vertColour, {u + uNorm, v + vNorm} };
                    verts.emplace_back(vert);
                }
            }
        }

        if (!verts.empty())
        {
            m_subsets.emplace_back();
            //m_subsets.back().texture = *textures[i];
            m_subsets.back().texture = textures[i];
            m_subsets.back().vertexData.swap(verts);
        }
    }

    return true;
}

void MapLayer::draw(engine::Renderer* renderer)
{
    assert(renderer);
    for (const auto& s : m_subsets)
    {
       // SDL_RenderGeometry(renderer, s.texture, s.vertexData.data(), static_cast<std::int32_t>(s.vertexData.size()), nullptr, 0);
        renderer->drawGeometry(s.texture, s.vertexData.data(), static_cast<std::int32_t>(s.vertexData.size()), nullptr, 0);
    }
}


}