#pragma once

#include "tmx_layer.h"

#include "engine/wrapper.h"

namespace engine {
    class Application;
    class Texture;
}

namespace tmx {

    class TmxMapLoader 
    {
    public:
        TmxMapLoader() = delete;
        TmxMapLoader(const TmxMapLoader&) = delete;
        TmxMapLoader(TmxMapLoader&&) = delete;
        
        TmxMapLoader(engine::Application& app);
        ~TmxMapLoader();

        bool load(const std::string& mapfile);

        void setPos(const engine::Vec2f& pos);

        void draw();

    private:
        engine::Application& _application;

        std::vector<std::unique_ptr<tmx::MapLayer>> _layers;
    };
}