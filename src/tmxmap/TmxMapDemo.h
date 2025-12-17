#pragma once

#include "TmxTexture.h"
#include "TmxLayer.h"

#include "engine/wrapper.h"

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

        void setPos(const engine::Vec2f& pos);

        void draw();

    private:
        engine::Application& _application;

        std::vector<engine::Texture*> _textures;

        std::vector<std::unique_ptr<tmx::MapLayer>> _layers;
    };
}