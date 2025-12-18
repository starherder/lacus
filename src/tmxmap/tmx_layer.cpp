
#include "tmx_layer.h"

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
    const auto tileSize = map.getTileSize();
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

        float margin = ts.getMargin();
        float space = ts.getSpacing();

        const auto& textTileSize = ts.getTileSize();
        const auto textSize = ts.getImageSize();
        auto textTileColumn = ts.getColumnCount();

        const float uNorm = (float)(textTileSize.x) / textSize.x;
        const float vNorm = (float)(textTileSize.y) / textSize.y;

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
            m_subsets.emplace_back();
            m_subsets.back().texture = textures[i];
            m_subsets.back().vertexData.swap(verts);
        }
    }

    return true;
}

void MapLayer::setPos(const engine::Vec2f& pos)
{
    for (auto& grid : m_subsets)
    {
        for (auto& vertex : grid.vertexData)
        {
            auto& point = vertex.position;
            point.x += pos.x;
            point.y += pos.y;
        }
    }
}

void MapLayer::draw(engine::Renderer* renderer)
{
    assert(renderer);
    for (const auto& s : m_subsets)
    {
        renderer->drawGeometry(s.texture, s.vertexData.data(), static_cast<std::int32_t>(s.vertexData.size()), nullptr, 0);
    }
}


}