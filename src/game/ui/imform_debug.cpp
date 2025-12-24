#include "imform_debug.h"
#include "spdlog/spdlog.h"

namespace game {

    
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
        if (ImGui::Button("what ?")) {
            spdlog::info("what ?");
        }

    }
    ImGui::End();
}

}