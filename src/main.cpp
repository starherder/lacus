#include "engine/application.h"

#include "samples/sample_plugin_manager.h"

int main(int argc, char* argv[]) {

    engine::Application app;

    app.init("res/system.json", "log/lacus.log");

    samples::SamplePluginManager::inst().init(app);

    app.run();

    return 0;
}