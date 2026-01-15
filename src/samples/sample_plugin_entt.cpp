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
    _camera = std::make_unique<SampleCamera>(Vec2{0, 0}, Vec2{window.getSize()});

    auto& eventDispatcher = application()->eventDispatcher();
    eventDispatcher.onSdlEvent.connect([this](const Event& e){ _camera->handleEvent(e); });
    
    initEntities();

    initDrawTest();
}


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
    //TestDataMgr mgr;
    //mgr.add("kitty", "i am a cat");
    //mgr.add("bark"_hs, "i am a dog");
    //mgr.add("pony"_hs, "i am a horse");
    //
    //spdlog::info("kitty = {}", mgr.get("kitty"));
    //spdlog::info("bark = {}", mgr.get("bark"_hs));
    //spdlog::info("pony = {}", mgr.get("pony"_hs));

    //mgr.output();
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
        _camera->update(application()->frameTicker().deltaSeconds());
    }
}

void SamplePluginEntt::onDraw()  
{
    drawTest();

    onEntityDrawSystem();
    
}

void SamplePluginEntt::onClose()  
{
    spdlog::info("Release sample plugin scene");
}

void SamplePluginEntt::initEntities()
{
    std::string imagename = "textures/UI/frame.png";
    _texture = application()->resourceManager().textureManager().load(HashString{imagename.c_str()});
    if(!_texture)
    {
        spdlog::error("load texture {} failed", imagename);
        return;
    }

    for (int x = 0; x < _xcount; x++)
    {
        for (int y = 0; y < _ycount; y++)
        {
            auto pos = Vec2f{ x * _gridw, y * _gridh };
            auto size = Vec2f{ _gridw, _gridh };

            auto color = Color{ HSVColor{(float)(x % 360), 1.0f, 1.0f, 1.0f } };

            Vertex v1{ {pos.x, pos.y},                   color, {0.0f,      0.0f} };
            Vertex v2{ {pos.x + size.x, pos.y},          color, {0.333333f, 0.0f} };
            Vertex v3{ {pos.x + size.x, pos.y + size.y}, color, {0.333333f, 1.0f} };
            Vertex v4{ {pos.x, pos.y + size.y},          color, {0.0f,      1.0f} };

            _vertexData.world_vertices.push_back(v1);
            _vertexData.world_vertices.push_back(v2);
            _vertexData.world_vertices.push_back(v3);
            _vertexData.world_vertices.push_back(v4);
            
            _vertexData.world_indices.push_back((x * _xcount + y) * 4 + 0);
            _vertexData.world_indices.push_back((x * _xcount + y) * 4 + 1);
            _vertexData.world_indices.push_back((x * _xcount + y) * 4 + 2);
            _vertexData.world_indices.push_back((x * _xcount + y) * 4 + 0);
            _vertexData.world_indices.push_back((x * _xcount + y) * 4 + 2);
            _vertexData.world_indices.push_back((x * _xcount + y) * 4 + 3);
        }
    }

    spdlog::info("create entity : count {}, vertex.size = {}", _xcount * _ycount, _vertexData.world_vertices.size());
}

void SamplePluginEntt::initDrawTest()
{
    _vdata.clear();
    _idata.clear();

    Color color = Color::Green;
    Vec2  size = {50, 50};

    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            Vec2 pos{x*50, y*50};

            Vertex v1{ {pos.x, pos.y},                   color, {0.0f,      0.0f} };
            Vertex v2{ {pos.x + size.x, pos.y},          color, {0.333333f, 0.0f} };
            Vertex v3{ {pos.x + size.x, pos.y + size.y}, color, {0.333333f, 1.0f} };
            Vertex v4{ {pos.x, pos.y + size.y},          color, {0.0f,      1.0f} };

            _vdata.push_back(v1);
            _vdata.push_back(v2);
            _vdata.push_back(v3);
            _vdata.push_back(v4);
            
            _idata.push_back((x * 3 + y) * 4 + 0);
            _idata.push_back((x * 3 + y) * 4 + 1);
            _idata.push_back((x * 3 + y) * 4 + 2);
            _idata.push_back((x * 3 + y) * 4 + 0);
            _idata.push_back((x * 3 + y) * 4 + 2);
            _idata.push_back((x * 3 + y) * 4 + 3);
        }
    }
}

void SamplePluginEntt::drawTest()
{
    auto& imPainter = application()->im_painter();
    //imPainter.drawTextureUV(_texture, { 0.0f, 0.0f, 0.33333f, 1.0f }, { 100,100,100,100 });

    Color color = Color::Green;
    Vec2  size = { 50, 50 };

    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            Vec2 pos{ x * 50, y * 50 };

            Vertex v1{ {pos.x, pos.y},                   color, {0.0f,      0.0f} };
            Vertex v2{ {pos.x + size.x, pos.y},          color, {0.333333f, 0.0f} };
            Vertex v3{ {pos.x + size.x, pos.y + size.y}, color, {0.333333f, 1.0f} };
            Vertex v4{ {pos.x, pos.y + size.y},          color, {0.0f,      1.0f} };

            _vdata.push_back(v1);
            _vdata.push_back(v2);
            _vdata.push_back(v3);
            _vdata.push_back(v4);

            _idata.push_back((x * 3 + y) * 4 + 0);
            _idata.push_back((x * 3 + y) * 4 + 1);
            _idata.push_back((x * 3 + y) * 4 + 2);
            _idata.push_back((x * 3 + y) * 4 + 0);
            _idata.push_back((x * 3 + y) * 4 + 2);
            _idata.push_back((x * 3 + y) * 4 + 3);
        }
    }

    imPainter.drawGeometry(_texture,
        _vdata.data(), _vdata.size(),
        _idata.data(), _idata.size(), {100, 300});
    
}

void SamplePluginEntt::onEntityDrawSystem()
{
    auto& render = application()->renderer();
    render.drawGeometry(_texture, _vertexData.world_vertices.data(), _vertexData.world_vertices.size(),
        _vertexData.world_indices.data(), _vertexData.world_indices.size());

#if 0
    auto& imPainter = application()->im_painter();
    
    imPainter.drawGeometry(_texture, 
                            _vertexData.world_vertices.data(), _vertexData.world_vertices.size(),
                            _vertexData.world_indices.data(), _vertexData.world_indices.size(),
                            -_camera->getPos());
#endif
}

}