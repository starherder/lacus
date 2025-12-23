#pragma once

#include "text_render.h"
#include "wrapper.h"
#include "texture.h"
#include <memory>

struct SDL_Renderer;

namespace engine {

class Texture;
class Font;

class Renderer final
{
public:
    // 构造函数和析构函数
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // 渲染器创建和销毁相关函数
    static int getNumRenderDrivers();
    static const char* getRenderDriver(int index);
    
    bool init(SDL_Window* window);
    
    bool setClipRect(const Rect& rect) const;
    Rect getClipRect() const;
    bool clipEnabled() const;
    
    bool setRenderScale(const Vec2& scale) const;
    Vec2 getRenderScale() const; 
    
    bool setDrawColor(const Color& color) const;

    Color getDrawColor() const;
    
    // 颜色缩放
    bool setColorScale(float scale) const;
    float getColorScale() const;
    
    // 绘制混合模式
    bool setDrawBlendMode(BlendMode blendMode) const;
    BlendMode getDrawBlendMode() const;
    
    bool drawPoint(const Vec2& point) const;
    bool drawPoints(const Vec2* points, int count) const;
    
    bool drawLine(const Vec2& start, const Vec2& end) const;
    bool drawlines(const Vec2* points, int count) const;
    
    bool drawRect(const Rect& rect) const;
    bool drawRects(const Rect* rects, int count) const;
    bool drawFillRect(const Rect& rect) const;
    bool drawFillRects(const Rect* rects, int count) const;
    
    // 纹理渲染
    bool drawTexture(Texture* texture, const Rect& srcrect, const Rect& dstrect) const;

    bool drawTextureRotated(Texture* texture, const Rect& srcrect, const Rect& dstrect, 
                            double angle, const Vec2& center, FlipMode flip) const;

    bool drawTextureAffine(Texture *texture, const Rect *srcrect, const Vec2 *origin,
                            const Vec2 *right, const Vec2 *down) const;

    bool drawTextureTiled(Texture* texture, const Rect* srcrect, float scale, const Rect* dstrect) const;

    bool drawTexture9Grid(Texture* texture, const Rect* srcrect, 
                           float left_width, float right_width, float top_height, float bottom_height, 
                           float scale, const Rect* dstrect) const;
    
    // 几何渲染
     bool drawGeometry(Texture* texture, const Vertex* vertices, int num_vertices, 
                        const int* indices, int num_indices) const;

    bool drawGeometryRaw(Texture* texture,  const float* xy, int xy_stride,
                        const Color& color, int color_stride, const float* uv, int uv_stride,
                        int num_vertices, const void* indices, int num_indices, int size_indices) const;
    
    // 字体渲染
    Vec2 getTextSize(const std::string& text, Font* font);
    bool drawText(const std::string& text, Font* font, const Vec2& pos, const Color& color=Color{255,255,255,255});
    bool drawDebugText(const Vec2& pos,const char* str) const;
    bool drawDebugTextFormat(const Vec2& pos, SDL_PRINTF_FORMAT_STRING const char* fmt, ...) const SDL_PRINTF_VARARG_FUNC(3);

    bool present() const;
    
    bool flushRenderer() const;

    bool clear() const;
    
    bool setRenderVSync(int vsync) const;
    bool getRenderVSync(int* vsync) const;
    
    auto getSdlRenderer() const { return _renderer; };
    
private:
    // 渲染器销毁
    void destroy();

    bool initTextRenderer();

private:
    SDL_Renderer* _renderer = nullptr;
    
    std::unique_ptr<TextRenderer> _textRenderer = nullptr;
};

} // namespace engine