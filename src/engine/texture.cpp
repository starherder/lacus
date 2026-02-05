#include "texture.h"
#include "render.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <tinyxml2/tinyxml2.h>

namespace engine {


     Texture::Texture(SDL_Texture* texture, const Vec2& size)
     {
        _texture = texture;
        _size = size;
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
         _texSetMap.insert({ "", TexSet{} });
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

        if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST)) {
            LogWarn("set texture scale mode to nearest failed");
            return nullptr;
        }

        Vec2 size = {0,0};
        if (!SDL_GetTextureSize(texture, &size.x, &size.y))
        {
            LogError("SDL_GetTextureSize failed.");
            return nullptr;
        }

        auto texturePtr = std::make_shared<Texture>(texture, size);
        auto [iter, res] = _textures.insert({id, texturePtr });
        if (!res) { 
            LogError("intser texture {} to map failed.", path.string());
            return nullptr; 
        }

        return texturePtr.get();
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

    TexSet* TextureManager::loadTexSet(const std::string& file)
    {
        using namespace tinyxml2;

        auto path = resPath() / file;
        if (!fs::exists(path))
        {
            LogError("texture tileset path({}) NOT exist.", file);
            return nullptr;
        }

        auto xmlDoc = std::make_shared<XMLDocument>();
        XMLError error = xmlDoc->LoadFile(path.string().c_str());
        if (error != XML_SUCCESS)
        {
            LogError("load texture tileset({}) failed.", path.string());
            return nullptr;
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
            if (texture)
            {
                auto tilenode = texnode->FirstChildElement("tile");
                while (tilenode)
                {
                    auto tilename = tilenode->Attribute("name");
                    auto rect = ToRect(tilenode->Attribute("rect"));

                    texset.tileset[tilename] = TexTile{texture, rect};
                    tilenode = tilenode->NextSiblingElement("tile");
                }
            }

            texnode = texnode->NextSiblingElement("texture");
        }

        auto [it, res] =_texSetMap.insert({texsetName, texset});
        if (!res) 
        {
            LogError("add tileset({}) to map failed.", path.string());
            nullptr; 
        }

        return &(it->second);
    }

    TexTile* TextureManager::getTexTile(const std::string& tile, const std::string& tileset)
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
            if (tileset.empty())
            {
                return loadTextureAsTile(tile);
            }

            LogError("texture tile ({}) NOT found in tileset ({})", tile, tileset);
            return nullptr;
        }

        return &(iter->second);
    }

    TexTile* TextureManager::loadTextureAsTile(const std::string& filepath)
    {
        auto texture = get(filepath);
        if (texture) 
        {
            // add to default texture tileset
            TexTile textile{ texture, Rect{Vec2{0,0}, texture->size()} };
            auto [it, res] = _texSetMap[""].tileset.insert({ filepath.data(), textile });
            if (!res) 
            {
                LogError("insert texture ({}) to map failed.", filepath);
                return nullptr;
            }

            return &(it->second);
        }

        LogError("load texture ({}) failed.", filepath);
        return nullptr;
    }

}

