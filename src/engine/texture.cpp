#include "texture.h"
#include "render.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <spdlog/spdlog.h>


namespace engine {


     Texture::Texture(const std::string& name, SDL_Texture* texture)
     {
        _name = name;
        _texture = texture;

        // 载入纹理时，设置纹理缩放模式为最邻近插值
        if (!SDL_SetTextureScaleMode(_texture, SDL_SCALEMODE_NEAREST)) {
            spdlog::warn("set texture scale mode to nearest failed");
        }
     }

     Texture::~Texture()
     {
        if (_texture) {
            SDL_DestroyTexture(_texture);
        }
     }

     Vec2f Texture::size() const
     {
        if (!_texture) 
        {
            return Vec2f{0,0};
        }

        Vec2f size;
        if (!SDL_GetTextureSize(_texture, &size.x, &size.y)) 
        {
            spdlog::error("{}: SDL_GetTextureSize failed.", _name);
            return Vec2f{0,0};
        }

        return size;
     }

     // ===============================================================

     TextureManager::TextureManager(Renderer& renderer) : _renderer(renderer)
     {
     }

    Texture* TextureManager::load(const std::string& name, const std::string& filepath)
    {
        auto it = _textures.find(name);
        if (it != _textures.end()) {
            spdlog::warn("Texture {} already loaded", name);
            return it->second.get();
        }

        auto path = fs::current_path() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("texture {}, path({}) NOT exist.", name, filepath);
            return nullptr;
        }
        
        auto texture = IMG_LoadTexture(_renderer.getSdlRenderer(), path.string().c_str());
        if (!texture) {
            spdlog::error("Failed to load texture {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _textures.insert({name, std::make_unique<Texture>(name, texture)});
        return res ? iter->second.get() : nullptr;
    }

    Texture* TextureManager::get(const std::string& name, const std::string& filepath)
    {
        auto it = _textures.find(name);
        if (it != _textures.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return load(name, filepath);
        }

        return nullptr;
    }

    void TextureManager::unload(const std::string& name)
    {
        auto it = _textures.find(name);
        if (it != _textures.end()) {
            spdlog::info("Unloaded texture {}", name);
            _textures.erase(it);
        }
        else {
            spdlog::warn("Texture {} not found", name);
        }
    }

    void TextureManager::clear()
    {
        spdlog::info("Unloaded all textures");
        _textures.clear();
    }

}

