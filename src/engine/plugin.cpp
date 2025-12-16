#include "plugin.h"

namespace engine {

    void Plugin::setEnable(bool enable)
    {
        _enabled = enable;

        if(enable) 
        {
            onEnable();
        }
        else
        {
            onDisable();
        }
    }

    bool Plugin::isEnabled()
    {
        return _enabled;
    }

    void Plugin::init()
    {
        spdlog::info("plugin({}) init. ", name());
        
        _initialized = true;

        onInit();

        setEnable(_enabled);
    }

    void Plugin::update()
    {
        if(!initialized()) 
        {
            init();
            return;
        }

        if(!isEnabled())
        {
            return;
        }

        //spdlog::info("plugin({}) update. ", name());
        onUpdate();
    }

    void Plugin::draw()
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //spdlog::info("plugin({}) draw. ", name());
        onDraw();
    }

    void Plugin::drawUI()
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //spdlog::info("plugin({}) draw. ", name());
        onDrawUI();
    }

    void Plugin::close()
    {
        if(!initialized()) 
        {
            return;
        }

        spdlog::info("plugin({}) closed. ", name());
        onClose();
    }
    
    void Plugin::install(Application* app) 
    {
        spdlog::info("plugin({}) install. ", name());
        _application = app;
        onInstall();
    }

    void Plugin::uninstall() 
    {
        spdlog::info("plugin({}) uninstall. ", name());
        onUninstall();
        _application = nullptr;
    }

    void Plugin::handleEvent(const Event& event)
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //spdlog::info("plugin({}) handle event. ", name());
        onEvent(event);
    }



}