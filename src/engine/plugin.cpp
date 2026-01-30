#include "plugin.h"

namespace engine {

    void Plugin::setEnable(bool enable)
    {
        _enabled = enable;
        LogInfo("plugin({}) {}. ", name(), _enabled?"enabled":"disabled");

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
        LogInfo("plugin({}) init. ", name());
        
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

        //LogInfo("plugin({}) update. ", name());
        onUpdate();
    }

    void Plugin::draw()
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //LogInfo("plugin({}) draw. ", name());
        onDraw();
    }

    void Plugin::drawUI()
    {
        if(!initialized() || !isEnabled()) 
        {
            return;
        }

        //LogInfo("plugin({}) draw. ", name());
        onDrawUI();
    }

    void Plugin::close()
    {
        if(!initialized()) 
        {
            return;
        }

        LogInfo("plugin({}) closed. ", name());
        onClose();
    }
    
    void Plugin::install(Application* app) 
    {
        LogInfo("plugin({}) install. ", name());
        _application = app;
        onInstall();
    }

    void Plugin::uninstall() 
    {
        LogInfo("plugin({}) uninstall. ", name());
        onUninstall();
        _application = nullptr;
    }


}