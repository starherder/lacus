#include "render.h"

#include "wrapper.h"
#include "text_render.h"
#include "texture.h"
#include "font.h"

#include <memory>

namespace engine {

// 构造函数和析构函数
Renderer::Renderer() 
{
}

Renderer::~Renderer() 
{
    destroy();
}

// 渲染器创建和销毁相关函数
int Renderer::getNumRenderDrivers() {
    return SDL_GetNumRenderDrivers();
}

const char* Renderer::getRenderDriver(int index) {
    return SDL_GetRenderDriver(index);
}

bool Renderer::init(SDL_Window* window) {
    if (_renderer) {
        return false;
    }

    _renderer = SDL_CreateRenderer(window, nullptr);
    if(!_renderer){
        spdlog::error("create render failed");
        return false;
    }

    auto res = initTextRenderer();
    if(!res){
        spdlog::error("create text render failed");
        return false;
    }

    setDrawBlendMode(BlendMode::Blend);
    return true;
}

// 裁剪区域
bool Renderer::setClipRect(const Rect& rect) const {
    SDL_Rect r = rect;
    return SDL_SetRenderClipRect(_renderer, &r);
}

Rect Renderer::getClipRect() const {
    SDL_Rect rect;
    SDL_GetRenderClipRect(_renderer, &rect);
    return rect;
}

bool Renderer::clipEnabled() const {
    return SDL_RenderClipEnabled(_renderer);
}

bool Renderer::setRenderScale(const Vec2& scale) const {
    return SDL_SetRenderScale(_renderer, scale.x, scale.y);
}

Vec2 Renderer::getRenderScale() const {
    Vec2 scale;
    SDL_GetRenderScale(_renderer, &scale.x, &scale.y);
    return scale;
}

bool Renderer::setDrawColor(const Color& color) const {
    return SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
}


Color Renderer::getDrawColor() const {
    Color color;
    SDL_GetRenderDrawColor(_renderer, &color.r, &color.g, &color.b, &color.a);
    return color;
}

bool Renderer::setColorScale(float scale) const {
    return SDL_SetRenderColorScale(_renderer, scale);
}

float Renderer::getColorScale() const {
    float scale = 0.0f;
    SDL_GetRenderColorScale(_renderer, &scale);
    return scale;
}

bool Renderer::setDrawBlendMode(BlendMode blendMode) const {
    return SDL_SetRenderDrawBlendMode(_renderer, (SDL_BlendMode)blendMode);
}

BlendMode Renderer::getDrawBlendMode() const {
    SDL_BlendMode blendMode;
    SDL_GetRenderDrawBlendMode(_renderer, &blendMode);
    return (BlendMode)blendMode;
}

bool Renderer::clear() const {
    return SDL_RenderClear(_renderer);
}

bool Renderer::drawPoint(const Vec2& point) const {
    return SDL_RenderPoint(_renderer, point.x, point.y);
}

bool Renderer::drawPoints(const Vec2* points, int count) const {
    SDL_FPoint* fpoints = (SDL_FPoint*)(points);
    return SDL_RenderPoints(_renderer, fpoints, count);
}

bool Renderer::drawLine(const Vec2& start, const Vec2& end) const {
    return SDL_RenderLine(_renderer, start.x, start.y, end.x, end.y);
}

bool Renderer::drawlines(const Vec2* points, int count) const {
    SDL_FPoint* fpoints = (SDL_FPoint*)(points);
    return SDL_RenderLines(_renderer, fpoints, count);
}

bool Renderer::drawRect(const Rect& rect) const {
    SDL_FRect r = rect;
    return SDL_RenderRect(_renderer, &r);
}

bool Renderer::drawRects(const Rect* rects, int count) const {
    return SDL_RenderRects(_renderer, (SDL_FRect*)rects, count);
}

bool Renderer::drawFillRect(const Rect& rect) const {
    SDL_FRect r = rect;
    return SDL_RenderFillRect(_renderer, &r);
}

bool Renderer::drawFillRects(const Rect* rects, int count) const {
    return SDL_RenderFillRects(_renderer, (SDL_FRect*)rects, count);
}

bool Renderer::drawTexture(Texture* texture, const Rect& src, const Rect& dst) const {
    if(!texture) {
        return false;
    }

    SDL_FRect srcrect = src;
    SDL_FRect dstrect = dst;
    return SDL_RenderTexture(_renderer, texture->_texture, &srcrect, &dstrect);
}

bool Renderer::drawTextureRotated(Texture* texture, const Rect& src, const Rect& dst, 
                                   double angle, const Vec2& center, FlipMode flip) const {
    if(!texture) {
        return false;
    }
    
    SDL_FRect srcrect = src;
    SDL_FRect dstrect = dst;
    SDL_FPoint fcenter = {center.x, center.y};
    return SDL_RenderTextureRotated(_renderer, texture->_texture, &srcrect, &dstrect, angle, &fcenter, flip);
}

bool Renderer::drawTextureAffine(Texture *texture, const Rect* srcrect, const Vec2* origin,
                                                     const Vec2* right, const Vec2* down) const
{
    if(!texture) {
        return false;
    }
    return SDL_RenderTextureAffine(_renderer, texture->_texture, (SDL_FRect*)srcrect, (SDL_FPoint*)origin, (SDL_FPoint*)right, (SDL_FPoint*)down);
}

bool Renderer::drawTextureTiled(Texture* texture, const Rect* srcrect, float scale, const Rect* dstrect) const {
    if(!texture) {
        return false;
    }
    return SDL_RenderTextureTiled(_renderer, texture->_texture, (SDL_FRect*)srcrect, scale, (SDL_FRect*)dstrect);
}

bool Renderer::drawTexture9Grid(Texture* texture, const Rect* srcrect, 
                                 float left_width, float right_width, float top_height, float bottom_height, 
                                 float scale, const Rect* dstrect) const {
    if(!texture) {
        return false;
    }
    return SDL_RenderTexture9Grid(_renderer, texture->_texture, (SDL_FRect*)srcrect, 
    left_width, right_width, top_height, bottom_height, scale, (SDL_FRect*)dstrect);
}

bool Renderer::drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices, 
                             const int* indices, int num_indices) const {
    if(!texture) {
        return false;
    }
    return SDL_RenderGeometry(_renderer, texture->_texture, vertices, num_vertices, indices, num_indices);
}

bool Renderer::drawGeometryRaw(Texture* texture, 
                                const float* xy, int xy_stride,
                                const Color& color, int color_stride,
                                const float* uv, int uv_stride,
                                int num_vertices, const void* indices, int num_indices, int size_indices) const {
    if(!texture) {
        return false;
    }

    auto sdlfcolor = (SDL_FColor)color;
    return SDL_RenderGeometryRaw(_renderer, texture->_texture, xy, xy_stride, &sdlfcolor, color_stride, uv, uv_stride, 
                                num_vertices, indices, num_indices, size_indices);
}

bool Renderer::drawDebugText(const Vec2& pos,const char* str) const {
    return SDL_RenderDebugText(_renderer, pos.x, pos.y, str);
}

bool Renderer::drawDebugTextFormat(const Vec2& pos, SDL_PRINTF_FORMAT_STRING const char* fmt, ...) const SDL_PRINTF_VARARG_FUNC(3) {
    va_list args;
    va_start(args, fmt);
    bool result = SDL_RenderDebugTextFormat(_renderer, pos.x, pos.y, fmt, args);
    va_end(args);
    return result;
}

bool Renderer::present() const {
    return SDL_RenderPresent(_renderer);
}

void Renderer::destroy() {
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
}

bool Renderer::flushRenderer() const {
    return SDL_FlushRenderer(_renderer);
}

bool Renderer::setRenderVSync(int vsync) const {
    return SDL_SetRenderVSync(_renderer, vsync);
}

bool Renderer::getRenderVSync(int* vsync) const {
    return SDL_GetRenderVSync(_renderer, vsync);
}

bool Renderer::initTextRenderer()
{
    _textRenderer = std::make_unique<TextRenderer>();
    return _textRenderer->init(this);
}

Vec2 Renderer::getTextSize(const std::string& text, Font* font) {
    if(!_textRenderer) {
        return {0,0};
    }
    return _textRenderer->getTextSize(text, font);
}

bool Renderer::drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color) {
    if(!_textRenderer) {
        return false;
    }
    return _textRenderer->drawText(text, font, pos, color);
}


} // namespace engine