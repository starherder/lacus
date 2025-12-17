#include "font.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


namespace engine {


     Font::Font(const std::string& name, int size, TTF_Font* font)
     {
        _name = name;
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

    Font* FontManager::load(const std::string& name, int size, const std::string& filepath)
    {
        auto it = _Fonts.find({name, size});
        if (it != _Fonts.end()) {
            spdlog::warn("Font {} already loaded", name);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            spdlog::error("Font {}, path({}) NOT exist.", name, filepath);
            return nullptr;
        }
        
        auto font = TTF_OpenFont(path.string().c_str(), size);
        if (!font) {
            spdlog::error("Failed to load Font {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto [iter, res] = _Fonts.insert({std::pair{name, size}, std::make_unique<Font>(name, size, font)});
        return res ? iter->second.get() : nullptr;
    }

    Font* FontManager::get(const std::string& name, int size, const std::string& filepath)
    {
        auto it = _Fonts.find({name, size});
        if (it != _Fonts.end()) 
        {
            return it->second.get();
        }

        if(!filepath.empty())
        {
            return load(name, size, filepath);
        }

        return nullptr;
    }

    void FontManager::unload(const std::string& name, int size)
    {
        auto it = _Fonts.find({name, size});
        if (it != _Fonts.end()) {
            spdlog::info("Unloaded Font {}", name);
            _Fonts.erase(it);
        }
        else {
            spdlog::warn("Font {} not found", name);
        }
    }

    void FontManager::clear()
    {
        spdlog::info("Unloaded all Fonts");
        _Fonts.clear();
    }

}

