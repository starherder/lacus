#pragma once

#include "sample_plugin_scene.h"

#include "engine/application.h"

#include <entt/entt.hpp>

//#include "imform/im_manager.h"
//#include "imform/im_form_demo.h"

namespace samples {

using namespace engine;

struct ComTransform 
{
    Vec2f pos = {0, 0};
    Vec2f size = {100, 100};
    Vec2f scale = {1, 1};
    float rotate = 0.0f;
};

struct ComDisplay 
{
    Color color;
};


const char* SamplePluginScene::name()  
{ 
    return "scene_test"; 
}

void SamplePluginScene::onInit()  
{
    spdlog::info("Init sample plugin scene");

    initEntities();
}

void SamplePluginScene::onInstall()  
{
    spdlog::info("install sample plugin scene");   
}

void SamplePluginScene::onUninstall()  
{
    spdlog::info("uninstall sample plugin scene");   
}

void SamplePluginScene::onUpdate()  
{
}

void SamplePluginScene::onDraw()  
{
    onEntityDrawSystem();
}

void SamplePluginScene::onClose()  
{
    spdlog::info("Release sample plugin scene");
}

void SamplePluginScene::initEntities()
{
    for(int x=0; x<_xcount; x++)
    {
        for(int y=0; y<_ycount; y++)
        {
            auto ent = _registry.create();
            auto pos = Vec2f{x*_gridw, y*_gridh};
            auto size = Vec2f{_gridw, _gridh};
            auto scale = Vec2f(1.0f, 1.0f);
            float rotate = 0.0f;
            _registry.emplace<ComTransform>(ent, pos, size, scale, rotate);

            //HSVColor hsv {(float)(x%360), 1.0f, (float )y/(_ycount-y), 1.0f };
            HSVColor hsv {(float)(x%360), 1.0f, 1.0f, 1.0f };
            auto color = ColorUtils::HSV_to_RGB(hsv);
            _registry.emplace<ComDisplay>(ent, color);
        }
    }

    spdlog::info("create entity : {}", _xcount * _ycount);
}

void SamplePluginScene::onEntityDrawSystem()
{
    auto& renderer = application()->renderer();
    auto& view = _registry.view<ComTransform, ComDisplay>();
    //spdlog::info("draw entity : {}", view.size_hint());

    for(auto& ent : view)
    {
        auto& dis = _registry.get<ComDisplay>(ent);
        auto& trans = _registry.get<ComTransform>(ent);
        
        FRect rect{trans.pos.x, trans.pos.y, trans.size.x, trans.size.y};
        
        Color bkcolor{dis.color.r, dis.color.g, dis.color.b, 100};
        renderer.setDrawColor(bkcolor);
        renderer.drawFillRect(rect);

        //Color border_color{dis.color.r, dis.color.g, dis.color.b, 255};
        //renderer.setDrawColor(border_color);
        //renderer.drawRect(rect);
    }
}

}