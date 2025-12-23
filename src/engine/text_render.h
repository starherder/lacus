#pragma once

#include "wrapper.h"

struct SDL_Renderer;
struct TTF_TextEngine;


namespace engine {

    class Font;
    class Renderer;

    class TextRenderer final {
    public:
        TextRenderer();
        ~TextRenderer();
        
        bool init(Renderer* renderer);

        Vec2 getTextSize(const std::string& text, Font* font);

        bool drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color={255,255,255,255});

    private:
        SDL_Renderer* _renderer = nullptr;

        TTF_TextEngine* _text_engine = nullptr;   
    };

}