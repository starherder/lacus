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

    public:
        ImFormDebug();
        ~ImFormDebug();
        
	protected:
		void draw() override;
    };

}