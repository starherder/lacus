#pragma once

#include "imgui_plugin.h"

namespace imgui
{  
    void addImguiPlugins(engine::Application& app)
    {
        app.addPlugin<ImGuiPlugin>();
    }
}