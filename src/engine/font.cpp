#include "font.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


namespace engine {


     Font::Font(IdType id, int size, TTF_Font* font)
     {
        _id = id;
        _size = size;
        _font = font;
     }

     Font::~Font()
     {
        if (_font) 
        {
            TTF_CloseFont(_font);
        }
     }


     // ===============================================================

     FontManager::FontManager()
     {
        if (!TTF_WasInit() && !TTF_Init()) {
            
            spdlog::error("ttf font init faild.");
            assert("ttf font init faild");
        }
     }

     FontManager::~FontManager()
     {
        clear();

        TTF_Quit();
     }

    Font* FontManager::load(IdType id, int size, const std::string_view& filepath)
    {
        auto it = _Fonts.find({id, size});
        if (it != _Fonts.end()) {
            spdlog::warn("Font {} already loaded", id);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("Font {}, path({}) NOT exist.", id, filepath);
            return nullptr;
        }
        
        auto font = TTF_OpenFont(path.string().c_str(), size);
        if (!font) {
            spdlog::error("Failed to load Font {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _Fonts.insert({std::pair{id, size}, std::make_unique<Font>(id, size, font)});
        return res ? iter->second.get() : nullptr;
    }
    
    Font* FontManager::load(const HashString& str, int size)
    {
        return load(str.value(), size, str.data());
    }

    Font* FontManager::get(IdType id, int size, const std::string_view& filepath)
    {
        auto it = _Fonts.find({id, size});
        if (it != _Fonts.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return load(id, size, filepath);
        }

        return nullptr;
    }

    Font* FontManager::get(const HashString& str, int size)
    {
        return get(str.value(), size, str.data());
    }

    void FontManager::unload(const HashString& str, int size)
    {
        auto it = _Fonts.find({str.value(), size});
        if (it != _Fonts.end()) {
            spdlog::info("Unloaded Font {}", str.data());
            _Fonts.erase(it);
        }
        else {
            spdlog::warn("Font {} not found", str.data());
        }
    }

    void FontManager::clear()
    {
        spdlog::info("Unloaded all Fonts");
        _Fonts.clear();
    }

}

