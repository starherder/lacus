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
    
    bool setRenderScale(const Vec2f& scale) const;
    Vec2f getRenderScale() const; 
    
    bool setDrawColor(const Color& color) const;
    bool setDrawFColor(const FColor& color) const;

    Color getDrawColor() const;
    FColor getDrawFColor() const;
    
    // 颜色缩放
    bool setColorScale(float scale) const;
    float getColorScale() const;
    
    // 绘制混合模式
    bool setDrawBlendMode(BlendMode blendMode) const;
    BlendMode getDrawBlendMode() const;
    
    bool drawPoint(const Vec2f& point) const;
    bool drawPoints(const Vec2f* points, int count) const;
    
    bool drawLine(const Vec2f& start, const Vec2f& end) const;
    bool drawlines(const Vec2f* points, int count) const;
    
    bool drawRect(const FRect& rect) const;
    bool drawRects(const FRect* rects, int count) const;
    bool drawFillRect(const FRect& rect) const;
    bool drawFillRects(const FRect* rects, int count) const;
    
    // 纹理渲染
    bool drawTexture(Texture* texture, const FRect& srcrect, const FRect& dstrect) const;
    bool drawTextureRotated(Texture* texture, const FRect* srcrect, const FRect* dstrect, 
                            double angle, const Vec2f* center, FlipMode flip) const;
    bool drawTextureAffine(Texture *texture, const FRect *srcrect, const Vec2f *origin,
                            const Vec2f *right, const Vec2f *down) const;
    bool drawTextureTiled(Texture* texture, const FRect* srcrect, float scale, const FRect* dstrect) const;
    bool drawTexture9Grid(Texture* texture, const FRect* srcrect, 
                           float left_width, float right_width, float top_height, float bottom_height, 
                           float scale, const SDL_FRect* dstrect) const;
    
    // 几何渲染
     bool drawGeometry(Texture* texture, const SDL_Vertex* vertices, int num_vertices, 
                        const int* indices, int num_indices) const;
    bool drawGeometryRaw(Texture* texture,  const float* xy, int xy_stride,
                        const FColor* color, int color_stride, const float* uv, int uv_stride,
                        int num_vertices, const void* indices, int num_indices, int size_indices) const;
    
    // 字体渲染
    bool drawText(const std::string& text, Font* font, const Vec2f& pos, const Color& color={255,255,255,255});
    bool drawDebugText(const Vec2f& pos,const char* str) const;
    bool drawDebugTextFormat(const Vec2f& pos, SDL_PRINTF_FORMAT_STRING const char* fmt, ...) const SDL_PRINTF_VARARG_FUNC(3);

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