#pragma once

#include "wrapper.h"

namespace engine {

    class Application;
    class TextureManager;

    class ResourceManager final
    {
    public:
        ResourceManager() = delete;
        ResourceManager(Application& app);
        ~ResourceManager();
    
        TextureManager& textureMgr() { return *_textureMgr; }

    private:
        std::unique_ptr<TextureManager> _textureMgr = nullptr;
        Application& _application;
    };


}