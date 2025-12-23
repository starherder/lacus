#include "text_render.h"
#include "render.h"
#include "font.h"
#include <spdlog/spdlog.h>
#include "SDL3_ttf/SDL_ttf.h"

namespace engine {

    TextRenderer::TextRenderer()
    {
    }

    TextRenderer::~TextRenderer()
    {
        if (_text_engine) 
        {
            TTF_DestroyRendererTextEngine(_text_engine);
            _text_engine = nullptr;
        }
    }

    bool TextRenderer::init(Renderer* renderer)
    {
        _renderer = renderer->getSdlRenderer();
        
        // 初始化 SDL_ttf
        if (!TTF_WasInit() && !TTF_Init() ) 
        {
            spdlog::error("init ttf failed: {}", SDL_GetError());
            return false;
        }

        _text_engine = TTF_CreateRendererTextEngine(_renderer);
        if (!_text_engine) 
        {
            spdlog::error("create render text engine failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    Vec2 TextRenderer::getTextSize(const std::string& text, Font* font)
    {
        if(!font) {
            return {0,0};
        }

        auto ttf = font->_font;
        if(!ttf) {
            return {0,0};
        }
        
        // 创建临时 TTF_Text 对象   (目前效率不高，未来可以考虑使用缓存优化)
        TTF_Text* temp_text_object = TTF_CreateText(_text_engine, ttf, text.c_str(), 0);
        if (!temp_text_object) {
            spdlog::error("create text failed. {}", SDL_GetError());
            return {0,0};
        }

        int w, h;
        TTF_GetTextSize(temp_text_object, &w, &h);
        TTF_DestroyText(temp_text_object);
        return {w, h};
    }

    // 使用指定字体绘制文字
    bool TextRenderer::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color)
    {
        if(!font) {
            return false;
        }

        auto ttf = font->_font;
        if(!ttf) {
            return false;
        }
        
        // 创建临时 TTF_Text 对象   (目前效率不高，未来可以考虑使用缓存优化)
        TTF_Text* temp_text_object = TTF_CreateText(_text_engine, ttf, text.c_str(), 0);
        if (!temp_text_object) {
            spdlog::error("create text failed. {}", SDL_GetError());
            return false;
        }

        TTF_SetTextColor(temp_text_object, color.r, color.g, color.b, color.a);
        if (!TTF_DrawRendererText(temp_text_object, pos.x, pos.y)) {
            spdlog::error("render text failed. {}", SDL_GetError());
        }

        // 销毁临时 TTF_Text 对象
        TTF_DestroyText(temp_text_object);
        return true;
    }
}