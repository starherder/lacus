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

    Texture(SDL_Texture* texture, const Vec2& size);
    ~Texture();

    const Vec2& size() const { return _size; }
    SDL_Texture* texture() const { return _texture; }

private:
    SDL_Texture* _texture = nullptr;
    Vec2 _size;
};

class TexTile
{
public:
    TexTile() {}
    TexTile(Texture* texture, const Rect& rect)
        : _texture(texture), _rect(rect) {}
    ~TexTile() {}

    Texture* texture() const { return _texture; }
    Rect rect() const { return _rect; };

private:
    Texture* _texture = nullptr;
    Rect _rect;
};

struct TexSet {
    std::string name;
    fs::path cfgfile;
    std::map<std::string, TexTile> tileset;
};


// 材质管理器
class TextureManager : public IResManager
{
    using TexturePtr = std::shared_ptr<Texture>;
    using TextureMap = std::unordered_map<IdType, TexturePtr>;
    using TextureSetMap = std::map<std::string, TexSet>;

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

    // load tileset config
    TexSet* loadTexSet(const std::string& xml_config);
    bool loadAllTexSets(const fs::path& path);

    // get tile in tileset
    // if tileset is empty, use get(...) to get whole Texture as TexTile
    TexTile* getTexTile(const std::string& tile, const std::string& tileset="");

    // get tile in tileset
    // cfgtile : formate "tileset:tile"
    TexTile* getCfgTexTile(const std::string& cfgtile);

    const TextureSetMap& getTexSets() const { return _texSetMap; }

    void clear();

private:
    void unload(IdType id);

    Texture* load(IdType id, const std::string_view& filepath);
    Texture* get(IdType id, const std::string_view& filepath="");

    TexTile* loadTextureAsTile(const std::string& filepath);

    TexSet* loadTexSetFile(const fs::path& cfgpath);

private:
    Renderer& _renderer;
    TextureMap _textures;

    TextureSetMap _texSetMap;
};





}