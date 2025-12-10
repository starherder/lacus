#pragma once

#include "sample_plugin_draw.h"

namespace samples
{
    void addSamplePlugins(engine::Application& app)
    {
        app.addPlugin<SamplePluginDraw>();
    }
    
}