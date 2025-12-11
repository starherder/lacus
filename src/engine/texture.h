#pragma once

#include "wrapper.h"


struct SDL_Texture;

namespace engine {

struct Renderer;


// 材质
class Texture {
    friend class Renderer;

public:
    Texture() = delete;
    Texture(const Texture& other) = default;
    Texture(const std::string& name, SDL_Texture* texture);
    ~Texture();

    Vec2f size() const;

private:
    SDL_Texture* _texture = nullptr;
    std::string _name;
};


// 材质管理器
class TextureManager 
{
    using TexturePtr = std::unique_ptr<Texture>;
    using TextureMap = std::unordered_map<std::string, TexturePtr>;

public:
    TextureManager() = delete;
    TextureManager(Renderer& render);
    ~TextureManager() = default;

    Texture* load(const std::string& name, const std::string& filepath);
    Texture* get(const std::string& name, const std::string& filepath="");

    void unload(const std::string& name);
    void clear();

private:
    Renderer& _renderer;

    TextureMap _textures;
};





}