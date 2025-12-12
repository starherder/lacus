#pragma once

#include "sample_plugin_scene.h"

#include "engine/application.h"

#include <entt/entt.hpp>

#include "engine/camera.h"

namespace samples {

using namespace engine;


class MyCamera : public Camera
{
public:

    MyCamera(const Vec2i& size) : engine::Camera(size)
    {
    }

    void Update(float delta) override
    {
        if(_vec.length() < 1e-8)
        {
            return;
        }

        Move(_vec * delta);
    }

    void moveCamera(const Vec2i& dir)
    {
        auto pos = GetPos();
        spdlog::info("move camera ({}, {}), pos = ({}, {})", dir.x, dir.y, pos.x, pos.y);

        _vec = dir;
        _vec *= _speed;
    }

    void stopCamera()
    {
        spdlog::info("stop camera.");
        _vec = {0, 0};
    }

    void moveHome()
    {
        SetPos({0, 0});
    }

    bool HandleEvent(const Event& event) override
    {
        if(event.type == SDL_EVENT_KEY_DOWN)
        {
            switch(event.key.key)
            {
                case SDLK_LEFT: {
                    moveCamera({-1, 0});
                }break;
                case SDLK_RIGHT: {
                    moveCamera({1, 0});
                }break;
                case SDLK_UP: {
                    moveCamera({0, -1});
                }break;
                case SDLK_DOWN: {
                    moveCamera({0, 1});
                }break;
                case SDLK_SPACE: {
                    moveHome();
                }break;
                default:{
                }break;
            }
        }
        else if(event.type == SDL_EVENT_KEY_UP)
        {
            stopCamera();
        }

        return true;
    }

private:
    float _speed = 200.0f;
    Vec2f _vec = {0.0f, 0.0f};
};



struct ComTransform 
{
    Vec2i pos = {0, 0};
    Vec2i size = {100, 100};

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

    auto& window = application()->window();
    _camera = std::make_unique<MyCamera>(window.getWindowSize());

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
    if(_camera)
    {
        _camera->Update(application()->fpsChecker().delta_time());
    }
}

void SamplePluginScene::onDraw()  
{
    onEntityDrawSystem();
}

void SamplePluginScene::onClose()  
{
    spdlog::info("Release sample plugin scene");
}

void SamplePluginScene::onEvent(const Event& event)
{
    if(_camera)
    {
        _camera->HandleEvent(event);
    }
}

void SamplePluginScene::initEntities()
{
    for(int x=0; x<_xcount; x++)
    {
        for(int y=0; y<_ycount; y++)
        {
            auto ent = _registry.create();
            auto pos = Vec2i{x*_gridw, y*_gridh};
            auto size = Vec2i{_gridw, _gridh};
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
        const auto& dis = _registry.get<ComDisplay>(ent);
        const auto& trans = _registry.get<ComTransform>(ent);
        
        const auto& pos = _camera->WorldToScreen(trans.pos);
        const auto& size = trans.size;

        FRect rect{(float)pos.x, (float)pos.y, (float)size.x, (float)size.y};
        
        Color bkcolor{dis.color.r, dis.color.g, dis.color.b, 200};
        renderer.setDrawColor(bkcolor);
        renderer.drawFillRect(rect);

        Color border_color{dis.color.r, dis.color.g, dis.color.b, 255};
        renderer.setDrawColor(border_color);
        renderer.drawRect(rect);
    }
}

}