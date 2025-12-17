#pragma once

#include "TmxTexture.h"
#include "TmxLayer.h"

#include <memory.h>
#include <vector>

namespace engine {
    class Application;
    class Texture;
}

namespace tmx {

    class TmxMapDemo 
    {
    public:
        TmxMapDemo() = delete;
        TmxMapDemo(const TmxMapDemo&) = delete;
        TmxMapDemo(TmxMapDemo&&) = delete;
        
        TmxMapDemo(engine::Application& app);
        ~TmxMapDemo();

        bool load(const std::string& mapfile);

        void draw();

    private:
        engine::Application& _application;

        std::vector<engine::Texture*> _textures;

        std::vector<std::unique_ptr<tmx::MapLayer>> _layers;
    };
}