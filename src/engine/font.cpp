#include "font.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


namespace engine {


     Font::Font(IdType _id, int _size)
     {
        id = _id;
        size = _size;
     }

     Font::~Font()
     {
        if (ttfFont)
        {
            TTF_CloseFont(ttfFont);
        }

#ifdef USE_IMGUI_AS_RENDER_ENGINE
        if (imFont)
        {
            //auto imFonts = ImGui::GetIO().Fonts;
            //imFonts->RemoveFont(imFont);
        }
#endif
     }


     // ===============================================================

     FontManager::FontManager()
     {
        if (!TTF_WasInit() && TTF_Init() != 0) {
            
            LogError("ttf font init faild.");
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
            LogWarn("Font {} already loaded", id);
            return it->second.get();
        }

        auto path = resPath() / filepath;
        if(!fs::exists(path))
        {
            LogError("Font {}, path({}) NOT exist.", id, filepath);
            return nullptr;
        }
        
        auto ttfFont = TTF_OpenFont(path.string().c_str(), (float)size);
        if (!ttfFont) {
            LogError("Failed to load Font {}: {}", path.string(), SDL_GetError());
            return nullptr;
        }

        auto pFont = std::make_shared<Font>(id, size);
        pFont->ttfFont = ttfFont;

#ifdef USE_IMGUI_AS_RENDER_ENGINE

        auto& io = ImGui::GetIO();
        auto imFonts = ImGui::GetIO().Fonts;
        auto imfont = imFonts->AddFontFromFileTTF(path.string().c_str(), (float)size, nullptr, imFonts->GetGlyphRangesChineseFull());
        if (!imfont)
        {
            LogError("load font {} failed. use default.", path.string());
        }

        pFont->imFont = imfont;
#endif
        _Fonts.insert({ std::pair{id, size}, pFont });
        return pFont.get();
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

    Font* FontManager::get(const std::string& str, int pointSize)
    {
        return get(HashString(str.c_str()), pointSize);
    }

    Font* FontManager::get(const HashString& str, int size)
    {
        return get(str.value(), size, str.data());
    }

    void FontManager::unload(const HashString& str, int size)
    {
        auto it = _Fonts.find({str.value(), size});
        if (it != _Fonts.end()) {
            LogInfo("Unloaded Font {}", str.data());
            _Fonts.erase(it);
        }
        else {
            LogWarn("Font {} not found", str.data());
        }
    }

    void FontManager::clear()
    {
        LogInfo("Unloaded all Fonts");
        _Fonts.clear();
    }

}

