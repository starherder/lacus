#pragma once

#include "font.h"
#include "misc.h"
#include "wrapper.h"

namespace engine {

    class Application;
    class TextureManager;
    class FontManager;
    class AudioManager;

    class ResourceManager final : public IResManager
    {
    public:
        ResourceManager() = delete;
        ResourceManager(Application& app);
        ~ResourceManager();
    
        void setResPath(const fs::path& path) override;

        TextureManager& textureManager() { return *_textureManager; }

        FontManager& fontManager() { return *_fontManager;}

        AudioManager& audioManager() { return *_audioManager; }

    private:

        std::unique_ptr<TextureManager> _textureManager = nullptr;
        std::unique_ptr<FontManager> _fontManager = nullptr;
        std::unique_ptr<AudioManager> _audioManager = nullptr;
        
        Application& _application;
    };


}