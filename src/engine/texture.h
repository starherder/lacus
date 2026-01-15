#pragma once

#include "wrapper.h"
#include "misc.h"

struct SDL_Texture;

namespace engine 
{

class Renderer;


// 材质
class Texture 
{
public:
    Texture() = delete;
    Texture(Texture&& other) = delete;
    Texture(const Texture& other) = delete;
    Texture(SDL_Texture* texture);
    ~Texture();

    const Vec2& size() const { return _size; }

    SDL_Texture* texture() const { return _texture; }

private:
    SDL_Texture* _texture = nullptr;

    Vec2 _size;
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

    Texture* get(const std::string& file);
    Texture* get(const HashString& file);
    
    Texture* load(const HashString& file);
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