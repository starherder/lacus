#pragma once

#include "misc.h"
#include "render.h"
#include "texture.h"
#include "window.h"
#include "plugin.h"
#include "config.h"
#include "resource.h"
#include "audio.h"
#include "audio_player.h"

#include "utility/unsort_map.h"
#include <map>

namespace engine {

class Application final 
{
    using PluginUniquePtr = std::unique_ptr<Plugin>;
    using PluginMap = std::multimap<PluginPriority, PluginUniquePtr>;

public:
    Application();
    ~Application();

    Application(const Application& app) = delete;
    Application& operator=(const Application& app) = delete;

    void run();

    template<typename T, typename... Args>
    T* addPlugin(PluginPriority priority, Args&&... args);

    Plugin* getPlugin(const std::string& name);

    bool removePlugin(const std::string& name);

    Renderer& renderer() { return *_renderer; }
    
    Window& window() { return *_window; }

    SystemConfig& systemConfig() { return _config; }

    ResourceManager& resourceManager() { return *_resourceMgr; } 

    AudioPlayer& audioPlayer() { return *_audioPlayer; }

    const FpsChecker& fpsChecker() { return _fps_checker; }

    fs::path runPath();
    fs::path resPath();

private:
    bool init();
    bool close();

    void input();
    void update();
    void draw();
    void drawUI();

    bool preFrame();
    bool postFrame();
    
    void processEvent(const Event& event);

    bool initConfig();
    bool initLog();
    bool initWindow();
    bool initRenderer();
    bool initAudioPlayer();

private:
    std::unique_ptr<Renderer> _renderer = nullptr;

    std::unique_ptr<Window> _window = nullptr;

    std::unique_ptr<ResourceManager> _resourceMgr = nullptr;

    std::unique_ptr<AudioPlayer> _audioPlayer = nullptr;

    PluginMap _plugins;

    SystemConfig _config;

    FpsChecker _fps_checker;
    
    fs::path _res_path;

    bool _running = true;
};


template<typename T, typename... Args>
T* Application::addPlugin(PluginPriority priority, Args&&... args) 
{
    auto new_plugin = std::make_unique<T>(std::forward<Args>(args)...);

    auto name = new_plugin->name();
    auto plugin = getPlugin(name);
    if(plugin)
    {
        spdlog::error("plugin ({}) already exist.", name);
        return dynamic_cast<T*>(plugin);
    }

    auto plugin_ptr = new_plugin.get();
    _plugins.emplace(priority, std::move(new_plugin));

    plugin_ptr->install(this);
    return plugin_ptr;
}


} // namespace engine