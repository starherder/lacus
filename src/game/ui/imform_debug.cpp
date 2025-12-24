#include "imform_debug.h"
#include "spdlog/spdlog.h"

namespace game 
{

ImFormDebug::ImFormDebug()
{
}

ImFormDebug::~ImFormDebug()
{
}

void ImFormDebug::draw()
{
    ImGui::Begin("debug");
    {
        static bool showCollision = false;
        if(ImGui::Checkbox("show collision", &showCollision)){
            on_show_collision_debug.emit(showCollision);
        }

        if (ImGui::Button("what ?")) {
            spdlog::info("what ?");
        }

    }
    ImGui::End();
}

}