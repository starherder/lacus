#pragma once

#include "wrapper.h"

namespace engine
{

enum PluginPriority
{
    Bottom,
    Default,
    Top,
};

class Plugin
{
    friend class Application;

public:
    Plugin() {}
    Plugin(Plugin&&) = delete;
    Plugin(const Plugin&) = delete;
    virtual ~Plugin() = default;
    
public:
    virtual const char* name() = 0;

    virtual void onInit() {}
    virtual void onUpdate() {}
    virtual void onDraw() {}
    virtual void onDrawUI() {}
    virtual void onClose() {}
    virtual void onEvent(const Event& event) {}

    virtual void onInstall() {}
    virtual void onUninstall() {}

    virtual void onEnable() {}
    virtual void onDisable() {}

    void init();

    void update();

    void draw();

    void drawUI();

    void close();
    
    void install(Application* app);

    void uninstall();

    void handleEvent(const Event& event);

    void setEnable(bool enable);

    bool isEnabled() const { return _enabled; }

    bool initialized() const { return _initialized; }

    Application* application() {  return _application; }\

private:
    Application* _application = nullptr;

    bool _initialized = false;
    bool _enabled = true;
};

}