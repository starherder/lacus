#pragma once

#include "wrapper.h"
#include "misc.h"

struct SDL_Texture;

namespace engine {

class Renderer;


// 材质
class Texture {
    friend class Renderer;

public:
    Texture() = delete;
    Texture(const Texture& other) = default;
    Texture(SDL_Texture* texture);
    ~Texture();

    const Vec2f& size() const { return _size; }

private:
    SDL_Texture* _texture = nullptr;
    Vec2f _size;
};


// 材质管理器
class TextureManager : public IResManager
{
    using TexturePtr = std::unique_ptr<Texture>;
    using TextureMap = std::unordered_map<IdType, TexturePtr>;

public:
    TextureManager() = delete;
    TextureManager(Renderer& render);
    TextureManager(TextureManager&&) = delete;
    TextureManager(const TextureManager&) = delete;
    ~TextureManager() = default;

    Texture* load(const HashString& file);
    Texture* get(const HashString& file);
    void unload(const HashString& file);

    void clear();

private:
    void unload(IdType id);
    Texture* load(IdType id, const std::string_view& filepath);
    Texture* get(IdType id, const std::string_view& filepath="");

private:
    Renderer& _renderer;
    TextureMap _textures;
};





}