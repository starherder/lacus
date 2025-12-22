#pragma once

#include "sample_plugin_entt.h"

#include "engine/application.h"

#include <entt/entt.hpp>


namespace samples {

using namespace engine;


struct ComTransform 
{
    Vec2i pos = {0, 0};
    Vec2i size = {100, 100};

    Vec2f scale = {1, 1};
    float rotate = 0.0f;
};

struct ComDisplay 
{
    FColor color;
};

void SamplePluginEntt::onInit()  
{
    spdlog::info("Init sample plugin scene");

    auto& window = application()->window();
    _camera = std::make_unique<SampleCamera>(Vec2{window.getSize()});

    auto& eventDispatcher = application()->eventDispatcher();
    eventDispatcher.onSdlEvent.connect([this](const Event& e){ _camera->handleEvent(e); });

    initEntities();
}

struct VertexData
{
    std::vector<Vertex> world_vertices;
    std::vector<Vertex> screen_vertices;
};

struct TestDataMgr {

    void add(IdType id, const std::string& value) {
        _values[id] = value;
    }
    
    void add(const HashString& str, const std::string& value) {

        spdlog::info("add: str = {}, value = {}", str.data(), value);

        add(str.value(), value);
    }

    const std::string& get(IdType id) {
        return _values[id];
    }

    const std::string& get(const HashString& str) {
        return get(str.value());
    }

    void output() {
        spdlog::info("----------------------------------");
        for(auto& item : _values)
        {
            spdlog::info("{} = {}", item.first, item.second);
        }
        spdlog::info("----------------------------------");
    }

    std::map<IdType, std::string> _values;
};

void SamplePluginEntt::onInstall()  
{
    TestDataMgr mgr;
    mgr.add("kitty", "i am a cat");
    mgr.add("bark"_hs, "i am a dog");
    mgr.add("pony"_hs, "i am a horse");

    spdlog::info("kitty = {}", mgr.get("kitty"));

    spdlog::info("bark = {}", mgr.get("bark"_hs));

    spdlog::info("pony = {}", mgr.get("pony"_hs));

    mgr.output();
}

void SamplePluginEntt::onUninstall()  
{
}

void SamplePluginEntt::onEnable()
{
}

void SamplePluginEntt::onDisable()
{
}

void SamplePluginEntt::onUpdate()  
{
    if(_camera)
    {
        _camera->update(application()->fpsChecker().deltaSeconds());
    }
}

void SamplePluginEntt::onDraw()  
{
    onEntityDrawSystem();
}

void SamplePluginEntt::onClose()  
{
    spdlog::info("Release sample plugin scene");
}

void SamplePluginEntt::initEntities()
{
    VertexData vdata;

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
            auto color = ColorUtils::HSV_to_FRGB(hsv);
            _registry.emplace<ComDisplay>(ent, color);

            Vertex v1{{pos.x, pos.y},                   color, {0, 0}};
            Vertex v2{{pos.x + size.x, pos.y},          color, {0.333333f,0}};
            Vertex v3{{pos.x + size.x, pos.y + size.y}, color, {0.333333f, 1.0f}};
            Vertex v4{{pos.x, pos.y + size.y},          color, {0, 1.0f}};

            vdata.world_vertices.push_back(v1);
            vdata.world_vertices.push_back(v2);
            vdata.world_vertices.push_back(v3);
            vdata.world_vertices.push_back(v1);
            vdata.world_vertices.push_back(v3);
            vdata.world_vertices.push_back(v4);
        }
    }

    _registry.ctx().emplace<VertexData>(vdata);

    _texture = application()->resourceManager().textureManager().load("textures/UI/frame.png"_hs);
    assert(_texture);

    spdlog::info("create entity : count {}, vertex.size = {}", _xcount * _ycount, vdata.world_vertices.size());
}

void SamplePluginEntt::onEntityDrawSystem()
{
    auto& renderer = application()->renderer();
    auto& vdata = _registry.ctx().get<VertexData>();

    vdata.screen_vertices.clear();
    for(auto& vertex : vdata.world_vertices)
    {
        const auto& pos = _camera->worldToScreen(Vec2{vertex.position.x, vertex.position.y});
        vdata.screen_vertices.emplace_back(Vertex{{pos.x, pos.y}, vertex.color, vertex.tex_coord});
    }

    renderer.drawGeometry(_texture, vdata.screen_vertices.data(), (int)vdata.screen_vertices.size(), nullptr, 0); 
}

}