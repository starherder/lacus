#include "resource.h"
#include "application.h"
#include "texture.h"   

namespace engine {
    
    ResourceManager::ResourceManager(Application& app) : _application(app)
    {
        _textureMgr = std::make_unique<TextureManager>(app.renderer());
    }

    ResourceManager::~ResourceManager()
    {
    }


}