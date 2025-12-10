
#pragma once

#include "engine_wrapper.h"

namespace engine {

class Renderer final
{

public:
    // 构造函数和析构函数
    Renderer();
    ~Renderer();

    // 禁止拷贝构造和赋值操作
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // 封装SDL_Renderer相关的函数
    
    // 渲染器创建和销毁相关函数
    static int getNumRenderDrivers();
    static const char* getRenderDriver(int index);
    
    // 渲染器
    bool createRenderer(SDL_Window* window, const char* name);
    bool createRendererWithProperties(SDL_PropertiesID props);
    
    // 渲染器属性获取
    SDL_PropertiesID getRendererProperties() const;
    
    // 渲染器输出尺寸
    bool getRenderOutputSize(int* w, int* h) const;
    bool getCurrentRenderOutputSize(int* w, int* h) const;
    
    // 纹理创建相关函数
    SDL_Texture* createTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h) const;
    SDL_Texture* createTextureFromSurface(SDL_Surface* surface) const;
    SDL_Texture* createTextureWithProperties(SDL_PropertiesID props) const;
    SDL_PropertiesID getTextureProperties(SDL_Texture* texture) const;
    SDL_Renderer* getRendererFromTexture(SDL_Texture* texture) const;
    bool getTextureSize(SDL_Texture* texture, float* w, float* h) const;
    
    // 纹理颜色和alpha调节
    bool setTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b) const;
    bool setTextureColorModFloat(SDL_Texture* texture, float r, float g, float b) const;
    bool getTextureColorMod(SDL_Texture* texture, Uint8* r, Uint8* g, Uint8* b) const;
    bool getTextureColorModFloat(SDL_Texture* texture, float* r, float* g, float* b) const;
    bool setTextureAlphaMod(SDL_Texture* texture, Uint8 alpha) const;
    bool setTextureAlphaModFloat(SDL_Texture* texture, float alpha) const;
    bool getTextureAlphaMod(SDL_Texture* texture, Uint8* alpha) const;
    bool getTextureAlphaModFloat(SDL_Texture* texture, float* alpha) const;
    
    // 纹理混合模式和缩放模式
    bool setTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blendMode) const;
    bool getTextureBlendMode(SDL_Texture* texture, SDL_BlendMode* blendMode) const;
    bool setTextureScaleMode(SDL_Texture* texture, SDL_ScaleMode scaleMode) const;
    bool getTextureScaleMode(SDL_Texture* texture, SDL_ScaleMode* scaleMode) const;
    
    // 纹理更新
    bool updateTexture(SDL_Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch) const;
    bool updateYUVTexture(SDL_Texture* texture, const SDL_Rect* rect, 
                         const Uint8* Yplane, int Ypitch,
                         const Uint8* Uplane, int Upitch,
                         const Uint8* Vplane, int Vpitch) const;
    bool updateNVTexture(SDL_Texture* texture, const SDL_Rect* rect,
                        const Uint8* Yplane, int Ypitch,
                        const Uint8* UVplane, int UVpitch) const;
    
    // 纹理锁定和解锁
    bool lockTexture(SDL_Texture* texture, const SDL_Rect* rect, void** pixels, int* pitch) const;
    bool lockTextureToSurface(SDL_Texture* texture, const SDL_Rect* rect, SDL_Surface** surface) const;
    void unlockTexture(SDL_Texture* texture) const;
    
    // 渲染目标设置
    bool setRenderTarget(SDL_Texture* texture) const;
    SDL_Texture* getRenderTarget() const;
    
    // 逻辑渲染呈现
    bool setRenderLogicalPresentation(int w, int h, SDL_RendererLogicalPresentation mode) const;
    bool getRenderLogicalPresentation(int* w, int* h, SDL_RendererLogicalPresentation* mode) const;
    bool getRenderLogicalPresentationRect(SDL_FRect* rect) const;
    
    // 坐标转换
    bool renderCoordinatesFromWindow(float window_x, float window_y, float* render_x, float* render_y) const;
    bool renderCoordinatesToWindow(float render_x, float render_y, float* window_x, float* window_y) const;
    bool convertEventToRenderCoordinates(Event* event) const;
    
    // 视口操作
    bool setRenderViewport(const SDL_Rect* rect) const;
    bool getRenderViewport(SDL_Rect* rect) const;
    bool renderViewportSet() const;
    bool getRenderSafeArea(SDL_Rect* rect) const;
    
    // 裁剪区域
    bool setRenderClipRect(const SDL_Rect* rect) const;
    bool getRenderClipRect(SDL_Rect* rect) const;
    bool renderClipEnabled() const;
    
    // 渲染缩放
    bool setRenderScale(float scaleX, float scaleY) const;
    bool getRenderScale(float* scaleX, float* scaleY) const;
    
    // 绘制颜色设置
    bool setRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;
    bool setRenderDrawColorFloat(float r, float g, float b, float a) const;
    bool getRenderDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a) const;
    bool getRenderDrawColorFloat(float* r, float* g, float* b, float* a) const;
    
    // 颜色缩放
    bool setRenderColorScale(float scale) const;
    bool getRenderColorScale(float* scale) const;
    
    // 绘制混合模式
    bool setRenderDrawBlendMode(SDL_BlendMode blendMode) const;
    bool getRenderDrawBlendMode(SDL_BlendMode* blendMode) const;
    
    // 渲染清除
    bool renderClear() const;
    
    // 点和线绘制
    bool renderPoint(float x, float y) const;
    bool renderPoints(const SDL_FPoint* points, int count) const;
    bool renderLine(float x1, float y1, float x2, float y2) const;
    bool renderLines(const SDL_FPoint* points, int count) const;
    
    // 矩形绘制
    bool renderRect(const SDL_FRect* rect) const;
    bool renderRects(const SDL_FRect* rects, int count) const;
    bool renderFillRect(const SDL_FRect* rect) const;
    bool renderFillRects(const SDL_FRect* rects, int count) const;
    
    // 纹理渲染
    bool renderTexture(SDL_Texture* texture, const SDL_FRect* srcrect, const SDL_FRect* dstrect) const;
    bool renderTextureRotated(SDL_Texture* texture, const SDL_FRect* srcrect, const SDL_FRect* dstrect, 
                             double angle, const SDL_FPoint* center, SDL_FlipMode flip) const;
    //bool renderTextureAffine(SDL_Texture* texture, const SDL_FRect* srcrect, const SDL_FPoint* points) const;
    bool renderTextureTiled(SDL_Texture* texture, const SDL_FRect* srcrect, float scale, const SDL_FRect* dstrect) const;
    bool renderTexture9Grid(SDL_Texture* texture, const SDL_FRect* srcrect, 
                           float left_width, float right_width, float top_height, float bottom_height, 
                           float scale, const SDL_FRect* dstrect) const;
    
    // 几何渲染
    bool renderGeometry(SDL_Texture* texture, const SDL_Vertex* vertices, int num_vertices, 
                       const int* indices, int num_indices) const;
    bool renderGeometryRaw(SDL_Texture* texture, 
                          const float* xy, int xy_stride,
                          const SDL_FColor* color, int color_stride,
                          const float* uv, int uv_stride,
                          int num_vertices, const void* indices, int num_indices, int size_indices) const;
    
    // 像素读取
    SDL_Surface* renderReadPixels(const SDL_Rect* rect) const;
    
    // 渲染呈现
    bool renderPresent() const;
    
    // 纹理销毁
    void destroyTexture(SDL_Texture* texture) const;
    
    // 渲染器刷新
    bool flushRenderer() const;
    
    // Metal相关
    void* getRenderMetalLayer() const;
    void* getRenderMetalCommandEncoder() const;
    
    // Vulkan同步信号量
    bool addVulkanRenderSemaphores(Uint32 wait_stage_mask, Sint64 wait_semaphore, Sint64 signal_semaphore) const;
    
    // VSync设置和获取
    bool setRenderVSync(int vsync) const;
    bool getRenderVSync(int* vsync) const;
    
    // 调试文本渲染
    bool renderDebugText(float x, float y, const char* str) const;
    bool renderDebugTextFormat(float x, float y, SDL_PRINTF_FORMAT_STRING const char* fmt, ...) const SDL_PRINTF_VARARG_FUNC(4);
    
    auto getSdlRenderer() const { return m_renderer; };
    
private:
    // 渲染器销毁
    void destroyRenderer();

private:
    SDL_Renderer* m_renderer = nullptr;

};

} // namespace engine