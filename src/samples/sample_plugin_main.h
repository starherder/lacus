#pragma once

#include "sample_plugin_draw.h"
#include "sample_plugin_audio.h"

namespace samples
{
    void addSamplePlugins(engine::Application& app)
    {
        app.addPlugin<SamplePluginDraw>();
        
        app.addPlugin<SamplePluginAudio>();
    }
    
}