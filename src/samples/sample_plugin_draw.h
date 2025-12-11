#pragma once

#include "engine/application.h"
#include "engine/widget.h"

namespace samples {

    class SamplePluginDraw final : public engine::Plugin 
    {
    public:
        SamplePluginDraw() = default;
        ~SamplePluginDraw() = default;

        const char* name() override 
        { 
            return "render_test"; 
        }

        void onInit() override 
        {
            spdlog::info("Init sample plugin draw");
            auto& renderer = application()->renderer();
            
            engine::WidgetManager::inst().init(renderer);
            engine::WidgetManager::inst().create_label(entt::null, "label");
        }

        void onInstall() override 
        {
            spdlog::info("install sample plugin draw");   
        }

        void onUninstall() override 
        {
            spdlog::info("uninstall sample plugin draw");   
        }

        void onUpdate() override 
        {
            //spdlog::info("Update sample render");
        }

        void onDraw() override 
        {
            drawShape();

            drawTexture();
                
            engine::WidgetManager::inst().draw();
        }

        void onClose() override 
        {
            spdlog::info("Release sample plugin draw");
        }

    private:
        void drawShape()
        {
            auto& renderer = application()->renderer();
            
            renderer.setDrawColor({255, 255, 0, 255});
            renderer.drawFillRect({500, 200, 150, 150});

            renderer.setDrawColor({0, 0, 255, 255});
            renderer.drawRect({ 200, 150, 200, 100 });

            renderer.setDrawColor({255, 155, 0, 255});
            renderer.drawDebugText({10,10}, "Hello,world!");
        }

        void drawTexture()
        {
            auto& renderer = application()->renderer();
            auto& resourceMgr = application()->resourceMgr();

            auto tex_1 = resourceMgr.textureMgr().get("test_1", "res/textures/Buildings/house3.png");
            if(tex_1)
            {
                auto tex_sz = tex_1->size();
                renderer.drawTexture(tex_1, {0, 0, tex_sz.x, tex_sz.y}, {50, 300, tex_sz.x,tex_sz.y} );
            }
        }
    };
}