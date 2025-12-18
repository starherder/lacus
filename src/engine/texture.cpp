#include "texture.h"
#include "render.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <spdlog/spdlog.h>


namespace engine {


     Texture::Texture(SDL_Texture* texture)
     {
        _texture = texture;

        // 载入纹理时，设置纹理缩放模式为最邻近插值
        if (!SDL_SetTextureScaleMode(_texture, SDL_SCALEMODE_NEAREST)) {
            spdlog::warn("set texture scale mode to nearest failed");
        }
        
        if (!SDL_GetTextureSize(_texture, &_size.x, &_size.y)) 
        {
            spdlog::error("SDL_GetTextureSize failed.");
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
            spdlog::warn("Texture {} already loaded", id);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("texture {}, path({}) NOT exist.", id, filepath);
            return nullptr;
        }
        
        auto texture = IMG_LoadTexture(_renderer.getSdlRenderer(), path.string().c_str());
        if (!texture) {
            spdlog::error("Failed to load texture {}: {}", path.string(), SDL_GetError());
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
            spdlog::info("Unload texture {}", id);
            _textures.erase(it);
        }
        else {
            spdlog::warn("Texture {} not found", id);
        }
    }

    Texture* TextureManager::load(const HashString& file)
    {
        return load(file.value(), file.data());
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
        spdlog::info("Unloaded all textures");
        _textures.clear();
    }

}

