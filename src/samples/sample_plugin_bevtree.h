#pragma once

#include "engine/application.h"

namespace samples {

    using namespace engine;

    class Actor {
    public:
        Actor() = default;
        ~Actor() = default;
        
        virtual void update() = 0;
        virtual void draw(Renderer& renderer) = 0;
    };

    class SamplePluginBevTree final : public engine::Plugin 
    {
    public:
        SamplePluginBevTree() = default;
        ~SamplePluginBevTree() = default;

        const char* name() override { return "sample_bevtree_plugin"; }

        void onInit() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;
        
        std::vector<std::shared_ptr<Actor>> _actors;

    };
}