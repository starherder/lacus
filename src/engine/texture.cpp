#include "texture.h"
#include "render.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <tinyxml2/tinyxml2.h>

namespace engine {


     Texture::Texture(SDL_Texture* texture)
     {
        _texture = texture;

        // 载入纹理时，设置纹理缩放模式为最邻近插值
        if (!SDL_SetTextureScaleMode(_texture, SDL_SCALEMODE_NEAREST)) {
            LogWarn("set texture scale mode to nearest failed");
        }
        
        if (!SDL_GetTextureSize(_texture, &_size.x, &_size.y)) 
        {
            LogError("SDL_GetTextureSize failed.");
        }
     }

     Texture::~Texture()
     {
        if (_texture) {
            SDL_DestroyTexture(_texture);
        }
     }

     // ===============================================================

     TextureManager::TextureManager(Renderer& renderer) : _renderer(renderer)
     {
     }

    Texture* TextureManager::load(IdType id, const std::string_view& filepath)
    {
        auto it = _textures.find(id);
        if (it != _textures.end()) {
            LogWarn("Texture {} already loaded", id);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            LogError("texture {}, path({}) NOT exist.", id, filepath);
            return nullptr;
        }

        SDL_Texture* texture = IMG_LoadTexture(_renderer.getSdlRenderer(), path.string().c_str());
        if (!texture) {
            LogError("Failed to load texture {}", path.string());
            return nullptr;
        }

        auto [iter, res] = _textures.insert({id, std::make_unique<Texture>(texture)});
        return res ? iter->second.get() : nullptr;
    }

    Texture* TextureManager::get(IdType id, const std::string_view& filepath)
    {
        auto it = _textures.find(id);
        if (it != _textures.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return load(id, filepath);
        }

        return nullptr;
    }

    void TextureManager::unload(IdType id)
    {
        auto it = _textures.find(id);
        if (it != _textures.end()) {
            LogInfo("Unload texture {}", id);
            _textures.erase(it);
        }
        else {
            LogWarn("Texture {} not found", id);
        }
    }

    Texture* TextureManager::load(const HashString& file)
    {
        return load(file.value(), file.data());
    }

    Texture* TextureManager::get(const std::string& file)
    {
        return get(HashString(file.c_str()));
    }

    Texture* TextureManager::get(const HashString& file)
    {
        return get(file.value(), file.data());
    }

    void TextureManager::unload(const HashString& file)
    {
        unload(file.value());
    }

    void TextureManager::clear()
    {
        LogInfo("Unloaded all textures");
        _textures.clear();
    }

    bool TextureManager::loadTexSet(const std::string& file)
    {
        using namespace tinyxml2;

        auto path = resPath() / file;
        if (!fs::exists(path))
        {
            LogError("texture tileset path({}) NOT exist.", file);
            return false;
        }

        auto xmlDoc = std::make_shared<XMLDocument>();
        XMLError error = xmlDoc->LoadFile(path.string().c_str());
        if (error != XML_SUCCESS)
        {
            LogError("load texture tileset({}) failed.", path.string());
            return false;
        }

        auto root = xmlDoc->RootElement();
        auto texsetName = root->Attribute("name");

        TexSet texset;
        texset.name = texsetName;
        texset.cfgfile = path;

        auto texnode = root->FirstChildElement("texture");
        while (texnode) 
        {
            auto texturename = texnode->Attribute("file");
            auto texture = get(texturename);
            if (!texture)
            {
                continue;
            }

            auto tilenode = texnode->FirstChildElement("tile");
            while (tilenode)
            {
                auto tilename = tilenode->Attribute("name");
                auto rect = ToRect(tilenode->Attribute("rect"));

                texset.tileset[tilename] = TexTile{texture, rect};
                tilenode = tilenode->NextSiblingElement("tile");
            }
            texnode = texnode->NextSiblingElement("texture");
        }

        _texSetMap.insert({texsetName, texset});
    }

    TexTile* TextureManager::getTexTile(const std::string& tileset, const std::string& tile)
    {
        auto it = _texSetMap.find(tileset);
        if (it == _texSetMap.end())
        {
            LogError("texture set ({}) NOT found.", tileset);
            return nullptr;
        }

        auto& tilset = it->second.tileset;
        auto iter = tilset.find(tile);
        if (iter == tilset.end())
        {
            LogError("texture tile ({}) NOT found in tileset ({})", tile, tileset);
            return nullptr;
        }

        return &(iter->second);
    }


}

