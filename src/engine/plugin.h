#pragma once

#include "engine_wrapper.h"

namespace engine
{

class Plugin
{
    friend class Application;

public:
    Plugin() {}
    virtual ~Plugin() = default;
    
public:
    Application* application() 
	{ 
        return _application; 
    }

    virtual void onInit() {}
    virtual void onUpdate() {}
    virtual void onDraw() {}
    virtual void onClose() {}
    virtual void onEvent(Event& event) {}

    virtual void onInstall() {}
    virtual void onUninstall() {}

    virtual const char* name() = 0 ;

private:
    void Install(Application* app) 
	{
        _application = app;
        onInstall();
    }

    void Uninstall() 
	{
        onUninstall();
        _application = nullptr;
    }

private:
    Application* _application = nullptr;
};

}