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

        static float motionSpeed = 10.0f;
        if(ImGui::SliderFloat("motion speed", &motionSpeed, 10.0f, 1000.0f)) {
            on_motion_speed_changed.emit(motionSpeed);
        }

        static bool motionStart = false;
        if(!motionStart) 
        {
            if(ImGui::Button("motion start")) 
            {
                motionStart = true;
                on_motion_start.emit(motionStart, motionSpeed);
            }
        }
        else 
        {
            if(ImGui::Button("motion stop")) 
            {
                motionStart = false;
                on_motion_start.emit(motionStart, motionSpeed);
            }
        }

        if(motionStart)
        {
            static bool motionPause = false;
            if(motionPause) 
            {
                if(ImGui::Button("motion resume")) {
                    motionPause = false;
                    on_motion_pause.emit(motionPause);
                }
            }
            else 
            {
                if(ImGui::Button("motion pause")) 
                {
                    motionPause = true;
                    on_motion_pause.emit(motionPause);
                }
            }
        }

    }
    ImGui::End();
}

}