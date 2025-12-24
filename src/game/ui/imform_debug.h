#pragma once

#include "imform/imform.h"
#include "imform/imform_manager.h"
#include "engine/wrapper.h"

namespace game 
{
    using namespace engine;
    
    class ImFormDebug : public imgui::ImForm
    {
    public:
        signals::Signal<bool> on_show_collision_debug;
        signals::Signal<bool> on_motion_start;
        signals::Signal<float> on_motion_speed_changed;

    public:
        ImFormDebug();
        ~ImFormDebug();
        
	protected:
		void draw() override;
    };

}