#pragma once

#include "engine/application.h"
#include "engine/widget.h"

namespace samples {

    class SamplePluginDraw final : public engine::Plugin 
    {
    public:
        SamplePluginDraw() = default;
        ~SamplePluginDraw() = default;

        const char* name() override { return "sample_draw_plugin"; }

        void onInit() override 
        {
            //auto& renderer = application()->renderer();
            //engine::WidgetManager::inst().init(renderer);
            //engine::WidgetManager::inst().create_label(entt::null, "label");
        }

        void onInstall() override 
        {  
        }

        void onUninstall() override 
        {
        }

        void onEnable() override
        {
        }

        void onDisable() override
        {
        }

        void onUpdate() override 
        {
        }

        void onDraw() override 
        {
            drawShape();

            drawTexture();

            drawText();
                
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
            auto& resourceMgr = application()->resourceManager();
            {
                auto tex = resourceMgr.textureManager().get("test_1", "textures/Buildings/house3.png");
                if(tex)
                {
                    auto tex_sz = tex->size();
                    renderer.drawTexture(tex, {0, 0, tex_sz.x, tex_sz.y}, {50, 300, tex_sz.x,tex_sz.y} );
                }
            }
        }

        void drawText()
        {
            auto& renderer = application()->renderer();
            auto& resourceMgr = application()->resourceManager();
            {
                auto font = resourceMgr.fontManager().get("vonwaon", 30, "fonts/VonwaonBitmap-16px.ttf");
                if(font)
                {
                    renderer.drawText("WTF! 妙~", font, {300, 300}, {255, 100, 0, 255});
                }
            }
            {
                auto font = resourceMgr.fontManager().get("msyh", 20, "fonts/msyh.ttf");
                if(font)
                {
                    renderer.drawText("就是这个feel，倍er爽~", font, {300, 350}, {255, 200, 0, 255});
                }
            }
        }
    };
}