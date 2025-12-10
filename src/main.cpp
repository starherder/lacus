#include "engine/application.h"

#include "samples/sample_plugin_main.h"
#include "imform/imgui_plugin_main.h"

int main(int argc, char* argv[]) {

    engine::Application app;

    // 测试插件
    samples::addSamplePlugins(app);

    // 测试ImGui
    imgui::addImguiPlugins(app);

    app.run();


    return 0;
}