#pragma once

#include "sample_plugin_entt.h"

#include "engine/application.h"

#include <entt/entt.hpp>

#include "imform/imform_manager.h"


namespace samples {

using namespace engine;



void ImFormEntt::onInit()
{
}

void ImFormEntt::draw()
{
    ImGui::Begin("entt");

    auto num = _plugin->registry().storage<entt::entity>().size();
    ImGui::Text("entt::entities num = %lu", num);

    ImGui::Separator();

    ImGui::Dummy({ 20, 20 });

    static bool visible = false;
    if (ImGui::Checkbox("draw", &visible))
    {
        _plugin->enableEntt(visible);
    }

    ImGui::End();
}


struct ComTransform 
{
    Vec2 pos = {0, 0};
    Vec2 size = {100, 100};

    Vec2 scale = {1, 1};
    float rotate = 0.0f;
};

struct ComDisplay 
{
    FColor color;
};



void SamplePluginEntt::enableEntt(bool enable)
{
    if (enable)
    {
        initEntities();
    }
    else
    {
        _registry.clear();
    }
}

void SamplePluginEntt::onInit()  
{
    auto& window = application()->window();
    _camera = std::make_unique<SampleCamera>(Vec2{0, 0}, Vec2{window.getSize()});

    auto& eventDispatcher = application()->eventDispatcher();
    eventDispatcher.onSdlEvent.connect([this](const Event& e){ _camera->handleEvent(e); });
}


void SamplePluginEntt::onInstall()  
{
}

void SamplePluginEntt::onUninstall()  
{
}

void SamplePluginEntt::onEnable()
{
    imgui::ImFormManager::inst().showForm<ImFormEntt>("form_entt", application(), this);
}

void SamplePluginEntt::onDisable()
{
    imgui::ImFormManager::inst().closeForm("form_entt");
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
    onEntityDrawSystem();
}

void SamplePluginEntt::onClose()  
{
    spdlog::info("Release sample plugin scene");
}

void SamplePluginEntt::initEntities()
{
    int xcount = 100, ycount = 100;
    float xsize = 30.0f, ysize = 30.0f;

    for (int x = 0; x < xcount; x++)
    {
        for (int y = 0; y < ycount; y++)
        {
            auto ent = _registry.create();
            ComTransform trans;
            trans.size = {xsize, ysize};
            trans.pos = {x*xsize, y*ysize};
            _registry.emplace<ComTransform>(ent, trans);

            auto color = Color{ HSVColor{(float)(x % 360), 1.0f, 1.0f, 1.0f } };
            ComDisplay disp{ color };
            _registry.emplace<ComDisplay>(ent, disp);
        }
    }
}

void SamplePluginEntt::onEntityDrawSystem()
{
    auto& imPainter = application()->im_painter();

    auto views = _registry.view<ComTransform, ComDisplay>();
    for (auto& ent : views)
    {
        auto& trans = views.get<ComTransform>(ent);
        auto& disp = views.get<ComDisplay>(ent);

        imPainter.drawRect(disp.color, { trans.pos-_camera->getPos(), trans.size}, 10.0f);
    }
}

}