#include "resource.h"
#include "application.h"
#include "misc.h"
#include "texture.h"   
#include "font.h"
#include "audio.h"


namespace engine {
    
    ResourceManager::ResourceManager(Application& app) : _application(app)
    {
        _textureManager = std::make_unique<TextureManager>(app.renderer());
        _fontManager = std::make_unique<FontManager>();
        _audioManager = std::make_unique<AudioManager>();
    }

    ResourceManager::~ResourceManager()
    {
    }

    void ResourceManager::setResPath(const fs::path& path)
    {
        IResManager::setResPath(path);

        _textureManager->setResPath(path);
        _fontManager->setResPath(path);
        _audioManager->setResPath(path);
    }

}