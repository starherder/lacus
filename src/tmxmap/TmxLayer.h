
#pragma once

#include "engine/wrapper.h"


namespace engine {
    class Texture;
    class Renderer;
}

namespace tmx {

class Map;

class MapLayer final
{
public:
    explicit MapLayer();

    bool create(const tmx::Map&, std::uint32_t index, const std::vector<engine::Texture*>& textures);

    void draw(engine::Renderer*);

private:
    struct Subset final
    {
        std::vector<engine::Vertex> vertexData;
        engine::Texture* texture = nullptr;
    };
    
    std::vector<Subset> m_subsets;
};


}