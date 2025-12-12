#pragma once

#include "wrapper.h"


struct TTF_Font;

namespace engine {

// 字体的key值是 name + size
using FontKey = std::pair<std::string, int>;
struct FontKeyHash {
    std::size_t operator()(const FontKey& key) const noexcept {

        // 采用C++20标准库的hash_combine实现思路
        std::size_t h1 = std::hash<std::string>{}(key.first);
        std::size_t h2 = std::hash<int>{}(key.second);

        // 推荐的哈希合并方式，参考boost::hash_combine
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};


// 字体
class Font {
    friend class TextRenderer;

public:
    Font() = delete;
    Font(const Font& other) = default;
    Font(const std::string& name, int size, TTF_Font* font);
    ~Font();

    int size() { return _size; }

private:
    TTF_Font* _font = nullptr;
    std::string _name;
    int _size;
};


// 字体管理器
class FontManager 
{
    using FontPtr = std::unique_ptr<Font>;
    using FontMap = std::unordered_map<FontKey, FontPtr, FontKeyHash>;

public:
    FontManager();
    FontManager(FontManager&&) = delete;
    FontManager(const FontManager&) = delete;
    ~FontManager();

    Font* load(const std::string& name, int size, const std::string& filepath);
    Font* get(const std::string& name, int size, const std::string& filepath="");

    void unload(const std::string& name, int size);
    void clear();

private:
    FontMap _Fonts;
};





}