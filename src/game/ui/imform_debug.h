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
        signals::Signal<float> on_motion_speed_changed;

        signals::Signal<bool> on_motion_pause;
        signals::Signal<bool, float> on_motion_start;

    public:
        ImFormDebug();
        ~ImFormDebug();
        
	protected:
		void draw() override;
    };

}