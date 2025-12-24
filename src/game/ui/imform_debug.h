#pragma once

#include "imform/imform.h"
#include "imform/imform_manager.h"

namespace game {

    class ImFormDebug : public imgui::ImForm
    {
    public:
        ImFormDebug();
        ~ImFormDebug();
        
	protected:
		void draw() override;
    };

}