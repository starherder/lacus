#pragma once

#include "misc.h"
#include "render.h"
#include "texture.h"
#include "window.h"
#include "plugin.h"
#include "config.h"
#include "resource.h"

namespace engine {

class Application final 
{
    using PluginUniquePtr = std::unique_ptr<Plugin>;
    using PluginMap = std::unordered_map<std::string, PluginUniquePtr>;

public:
    Application();
    ~Application();

    Application(const Application& app) = delete;
    Application& operator=(const Application& app) = delete;

    void run();

    template<typename T, typename... Args>
    T* addPlugin(Args&&... args);

    bool hasPlugin(const std::string& name);

    bool removePlugin(const std::string& name);

    Renderer& renderer() { return *_renderer; }
    
    Window& window() { return *_window; }

    SystemConfig& systemConfig() { return _config; }

    ResourceManager& resourceMgr() { return *_resourceMgr; } 

    fs::path runPath();
    fs::path resPath();

private:
    bool init();
    bool close();

    void input();
    void update();
    void draw();

    void processEvent(Event& event);

    bool preFrame();
    bool postFrame();
    
    bool initConfig();
    bool initLog();
    bool initWindow();

private:
    std::unique_ptr<Renderer> _renderer = nullptr;

    std::unique_ptr<Window> _window = nullptr;

    std::unique_ptr<ResourceManager> _resourceMgr = nullptr;

    PluginMap _plugins;

    SystemConfig _config;

    FrameChecker _frame_checker;
    
    fs::path _res_path;

    bool _running = true;
};



template<typename T, typename... Args>
T* Application::addPlugin(Args&&... args) {
    auto plugin = std::make_unique<T>(std::forward<Args>(args)...);
    auto name = plugin->name();
    
    _plugins[name] = std::move(plugin);
    _plugins[name]->Install(this);

    return static_cast<T*>(_plugins[name].get());
}

} // namespace engine