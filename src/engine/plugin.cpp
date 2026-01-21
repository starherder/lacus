#include "plugin.h"

namespace engine {

    void Plugin::setEnable(bool enable)
    {
        _enabled = enable;
        SPDLOG_INFO("plugin({}) {}. ", name(), _enabled?"enabled":"disabled");

        if(enable) 
        {
            onEnable();
        }
        else
        {
            onDisable();
        }
    }

    void Plugin::init()
    {
        SPDLOG_INFO("plugin({}) init. ", name());
        
        _initialized = true;

        onInit();
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

        //SPDLOG_INFO("plugin({}) update. ", name());
        onUpdate();
    }

    void Plugin::draw()
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //SPDLOG_INFO("plugin({}) draw. ", name());
        onDraw();
    }

    void Plugin::drawUI()
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //SPDLOG_INFO("plugin({}) draw. ", name());
        onDrawUI();
    }

    void Plugin::close()
    {
        if(!initialized()) 
        {
            return;
        }

        SPDLOG_INFO("plugin({}) closed. ", name());
        onClose();
    }
    
    void Plugin::install(Application* app) 
    {
        SPDLOG_INFO("plugin({}) install. ", name());
        _application = app;
        onInstall();
    }

    void Plugin::uninstall() 
    {
        SPDLOG_INFO("plugin({}) uninstall. ", name());
        onUninstall();
        _application = nullptr;
    }


}