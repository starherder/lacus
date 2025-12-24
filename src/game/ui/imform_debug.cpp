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
        if(ImGui::Checkbox("show grid", &showCollision))
        {
            on_show_collision_debug.emit(showCollision);
        }

        static bool motionRun = false;
        if(ImGui::Checkbox("motion run", &motionRun))
        {
            on_motion_start.emit(motionRun);
        }

        static float motionSpeed = 10.0f;
        if(ImGui::SliderFloat("motion speed", &motionSpeed, 10.0f, 1000.0f))
        {
            on_motion_speed_changed.emit(motionSpeed);
        }

        if (ImGui::Button("what ?")) 
        {
            spdlog::info("what ?");
        }

    }
    ImGui::End();
}

}