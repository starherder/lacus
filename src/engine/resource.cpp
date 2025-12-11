#include "resource.h"
#include "application.h"
#include "texture.h"   
#include "font.h"


namespace engine {
    
    ResourceManager::ResourceManager(Application& app) : _application(app)
    {
        _textureManager = std::make_unique<TextureManager>(app.renderer());
        _fontManager = std::make_unique<FontManager>();
    }

    ResourceManager::~ResourceManager()
    {
    }


}