#pragma once

#include "font.h"
#include "misc.h"
#include "wrapper.h"

namespace engine {

    class Application;
    class TextureManager;
    class FontManager;
    class AudioManager;
    class AnimationManager;

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

        AnimationManager& animationManager() { return *_animationManager; }

    private:

        std::unique_ptr<TextureManager> _textureManager = nullptr;
        std::unique_ptr<FontManager> _fontManager = nullptr;
        std::unique_ptr<AudioManager> _audioManager = nullptr;
        std::unique_ptr<AnimationManager> _animationManager = nullptr;

        Application& _application;
    };


}