#pragma once

#include "font.h"
#include "wrapper.h"

namespace engine {

    class Application;
    class TextureManager;
    class FontManager;

    class ResourceManager final
    {
    public:
        ResourceManager() = delete;
        ResourceManager(Application& app);
        ~ResourceManager();
    
        TextureManager& textureManager() { return *_textureManager; }
        
        FontManager& fontManager() { return *_fontManager;}

    private:
        std::unique_ptr<TextureManager> _textureManager = nullptr;
        std::unique_ptr<FontManager> _fontManager = nullptr;
        Application& _application;
    };


}