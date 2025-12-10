#include "render.h"

namespace engine {

// 构造函数和析构函数
Renderer::Renderer() 
{

}

Renderer::~Renderer() 
{
    destroyRenderer();
}

// 渲染器创建和销毁相关函数
int Renderer::getNumRenderDrivers() {
    return SDL_GetNumRenderDrivers();
}

const char* Renderer::getRenderDriver(int index) {
    return SDL_GetRenderDriver(index);
}

bool Renderer::createRenderer(SDL_Window* window, const char* name) {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }
    m_renderer = SDL_CreateRenderer(window, name);
    return m_renderer != nullptr;
}

bool Renderer::createRendererWithProperties(SDL_PropertiesID props) {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }
    m_renderer = SDL_CreateRendererWithProperties(props);
    return m_renderer != nullptr;
}

SDL_PropertiesID Renderer::getRendererProperties() const {
    return SDL_GetRendererProperties(m_renderer);
}

bool Renderer::getRenderOutputSize(int* w, int* h) const {
    return SDL_GetRenderOutputSize(m_renderer, w, h);
}

bool Renderer::getCurrentRenderOutputSize(int* w, int* h) const {
    return SDL_GetCurrentRenderOutputSize(m_renderer, w, h);
}

SDL_Texture* Renderer::createTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h) const {
    return SDL_CreateTexture(m_renderer, format, access, w, h);
}

SDL_Texture* Renderer::createTextureFromSurface(SDL_Surface* surface) const {
    return SDL_CreateTextureFromSurface(m_renderer, surface);
}

SDL_Texture* Renderer::createTextureWithProperties(SDL_PropertiesID props) const {
    return SDL_CreateTextureWithProperties(m_renderer, props);
}

SDL_PropertiesID Renderer::getTextureProperties(SDL_Texture* texture) const {
    return SDL_GetTextureProperties(texture);
}

SDL_Renderer* Renderer::getRendererFromTexture(SDL_Texture* texture) const {
    return SDL_GetRendererFromTexture(texture);
}

bool Renderer::getTextureSize(SDL_Texture* texture, float* w, float* h) const {
    return SDL_GetTextureSize(texture, w, h);
}

bool Renderer::setTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b) const {
    return SDL_SetTextureColorMod(texture, r, g, b);
}

bool Renderer::setTextureColorModFloat(SDL_Texture* texture, float r, float g, float b) const {
    return SDL_SetTextureColorModFloat(texture, r, g, b);
}

bool Renderer::getTextureColorMod(SDL_Texture* texture, Uint8* r, Uint8* g, Uint8* b) const {
    return SDL_GetTextureColorMod(texture, r, g, b);
}

bool Renderer::getTextureColorModFloat(SDL_Texture* texture, float* r, float* g, float* b) const {
    return SDL_GetTextureColorModFloat(texture, r, g, b);
}

bool Renderer::setTextureAlphaMod(SDL_Texture* texture, Uint8 alpha) const {
    return SDL_SetTextureAlphaMod(texture, alpha);
}

bool Renderer::setTextureAlphaModFloat(SDL_Texture* texture, float alpha) const {
    return SDL_SetTextureAlphaModFloat(texture, alpha);
}

bool Renderer::getTextureAlphaMod(SDL_Texture* texture, Uint8* alpha) const {
    return SDL_GetTextureAlphaMod(texture, alpha);
}

bool Renderer::getTextureAlphaModFloat(SDL_Texture* texture, float* alpha) const {
    return SDL_GetTextureAlphaModFloat(texture, alpha);
}

bool Renderer::setTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blendMode) const {
    return SDL_SetTextureBlendMode(texture, blendMode);
}

bool Renderer::getTextureBlendMode(SDL_Texture* texture, SDL_BlendMode* blendMode) const {
    return SDL_GetTextureBlendMode(texture, blendMode);
}

bool Renderer::setTextureScaleMode(SDL_Texture* texture, SDL_ScaleMode scaleMode) const {
    return SDL_SetTextureScaleMode(texture, scaleMode);
}

bool Renderer::getTextureScaleMode(SDL_Texture* texture, SDL_ScaleMode* scaleMode) const {
    return SDL_GetTextureScaleMode(texture, scaleMode);
}

bool Renderer::updateTexture(SDL_Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch) const {
    return SDL_UpdateTexture(texture, rect, pixels, pitch);
}

bool Renderer::updateYUVTexture(SDL_Texture* texture, const SDL_Rect* rect, 
                               const Uint8* Yplane, int Ypitch,
                               const Uint8* Uplane, int Upitch,
                               const Uint8* Vplane, int Vpitch) const {
    return SDL_UpdateYUVTexture(texture, rect, Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch);
}

bool Renderer::updateNVTexture(SDL_Texture* texture, const SDL_Rect* rect,
                              const Uint8* Yplane, int Ypitch,
                              const Uint8* UVplane, int UVpitch) const {
    return SDL_UpdateNVTexture(texture, rect, Yplane, Ypitch, UVplane, UVpitch);
}

bool Renderer::lockTexture(SDL_Texture* texture, const SDL_Rect* rect, void** pixels, int* pitch) const {
    return SDL_LockTexture(texture, rect, pixels, pitch);
}

bool Renderer::lockTextureToSurface(SDL_Texture* texture, const SDL_Rect* rect, SDL_Surface** surface) const {
    return SDL_LockTextureToSurface(texture, rect, surface);
}

void Renderer::unlockTexture(SDL_Texture* texture) const {
    SDL_UnlockTexture(texture);
}

bool Renderer::setRenderTarget(SDL_Texture* texture) const {
    return SDL_SetRenderTarget(m_renderer, texture);
}

SDL_Texture* Renderer::getRenderTarget() const {
    return SDL_GetRenderTarget(m_renderer);
}

bool Renderer::setRenderLogicalPresentation(int w, int h, SDL_RendererLogicalPresentation mode) const {
    return SDL_SetRenderLogicalPresentation(m_renderer, w, h, mode);
}

bool Renderer::getRenderLogicalPresentation(int* w, int* h, SDL_RendererLogicalPresentation* mode) const {
    return SDL_GetRenderLogicalPresentation(m_renderer, w, h, mode);
}

bool Renderer::getRenderLogicalPresentationRect(SDL_FRect* rect) const {
    return SDL_GetRenderLogicalPresentationRect(m_renderer, rect);
}

bool Renderer::renderCoordinatesFromWindow(float window_x, float window_y, float* render_x, float* render_y) const {
    return SDL_RenderCoordinatesFromWindow(m_renderer, window_x, window_y, render_x, render_y);
}

bool Renderer::renderCoordinatesToWindow(float render_x, float render_y, float* window_x, float* window_y) const {
    return SDL_RenderCoordinatesToWindow(m_renderer, render_x, render_y, window_x, window_y);
}

bool Renderer::convertEventToRenderCoordinates(Event* event) const {
    return SDL_ConvertEventToRenderCoordinates(m_renderer, event);
}

bool Renderer::setRenderViewport(const SDL_Rect* rect) const {
    return SDL_SetRenderViewport(m_renderer, rect);
}

bool Renderer::getRenderViewport(SDL_Rect* rect) const {
    return SDL_GetRenderViewport(m_renderer, rect);
}

bool Renderer::renderViewportSet() const {
    return SDL_RenderViewportSet(m_renderer);
}

bool Renderer::getRenderSafeArea(SDL_Rect* rect) const {
    return SDL_GetRenderSafeArea(m_renderer, rect);
}

bool Renderer::setRenderClipRect(const SDL_Rect* rect) const {
    return SDL_SetRenderClipRect(m_renderer, rect);
}

bool Renderer::getRenderClipRect(SDL_Rect* rect) const {
    return SDL_GetRenderClipRect(m_renderer, rect);
}

bool Renderer::renderClipEnabled() const {
    return SDL_RenderClipEnabled(m_renderer);
}

bool Renderer::setRenderScale(float scaleX, float scaleY) const {
    return SDL_SetRenderScale(m_renderer, scaleX, scaleY);
}

bool Renderer::getRenderScale(float* scaleX, float* scaleY) const {
    return SDL_GetRenderScale(m_renderer, scaleX, scaleY);
}

bool Renderer::setRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    return SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
}

bool Renderer::setRenderDrawColorFloat(float r, float g, float b, float a) const {
    return SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a);
}

bool Renderer::getRenderDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a) const {
    return SDL_GetRenderDrawColor(m_renderer, r, g, b, a);
}

bool Renderer::getRenderDrawColorFloat(float* r, float* g, float* b, float* a) const {
    return SDL_GetRenderDrawColorFloat(m_renderer, r, g, b, a);
}

bool Renderer::setRenderColorScale(float scale) const {
    return SDL_SetRenderColorScale(m_renderer, scale);
}

bool Renderer::getRenderColorScale(float* scale) const {
    return SDL_GetRenderColorScale(m_renderer, scale);
}

bool Renderer::setRenderDrawBlendMode(SDL_BlendMode blendMode) const {
    return SDL_SetRenderDrawBlendMode(m_renderer, blendMode);
}

bool Renderer::getRenderDrawBlendMode(SDL_BlendMode* blendMode) const {
    return SDL_GetRenderDrawBlendMode(m_renderer, blendMode);
}

bool Renderer::renderClear() const {
    return SDL_RenderClear(m_renderer);
}

bool Renderer::renderPoint(float x, float y) const {
    return SDL_RenderPoint(m_renderer, x, y);
}

bool Renderer::renderPoints(const SDL_FPoint* points, int count) const {
    return SDL_RenderPoints(m_renderer, points, count);
}

bool Renderer::renderLine(float x1, float y1, float x2, float y2) const {
    return SDL_RenderLine(m_renderer, x1, y1, x2, y2);
}

bool Renderer::renderLines(const SDL_FPoint* points, int count) const {
    return SDL_RenderLines(m_renderer, points, count);
}

bool Renderer::renderRect(const SDL_FRect* rect) const {
    return SDL_RenderRect(m_renderer, rect);
}

bool Renderer::renderRects(const SDL_FRect* rects, int count) const {
    return SDL_RenderRects(m_renderer, rects, count);
}

bool Renderer::renderFillRect(const SDL_FRect* rect) const {
    return SDL_RenderFillRect(m_renderer, rect);
}

bool Renderer::renderFillRects(const SDL_FRect* rects, int count) const {
    return SDL_RenderFillRects(m_renderer, rects, count);
}

bool Renderer::renderTexture(SDL_Texture* texture, const SDL_FRect* srcrect, const SDL_FRect* dstrect) const {
    return SDL_RenderTexture(m_renderer, texture, srcrect, dstrect);
}

bool Renderer::renderTextureRotated(SDL_Texture* texture, const SDL_FRect* srcrect, const SDL_FRect* dstrect, 
                                   double angle, const SDL_FPoint* center, SDL_FlipMode flip) const {
    return SDL_RenderTextureRotated(m_renderer, texture, srcrect, dstrect, angle, center, flip);
}

//bool Renderer::renderTextureAffine(SDL_Texture* texture, const SDL_FRect* srcrect, const SDL_FPoint* points) const {
//    return SDL_RenderTextureAffine(m_renderer, texture, srcrect, points);
//}

bool Renderer::renderTextureTiled(SDL_Texture* texture, const SDL_FRect* srcrect, float scale, const SDL_FRect* dstrect) const {
    return SDL_RenderTextureTiled(m_renderer, texture, srcrect, scale, dstrect);
}

bool Renderer::renderTexture9Grid(SDL_Texture* texture, const SDL_FRect* srcrect, 
                                 float left_width, float right_width, float top_height, float bottom_height, 
                                 float scale, const SDL_FRect* dstrect) const {
    return SDL_RenderTexture9Grid(m_renderer, texture, srcrect, left_width, right_width, top_height, bottom_height, scale, dstrect);
}

bool Renderer::renderGeometry(SDL_Texture* texture, const SDL_Vertex* vertices, int num_vertices, 
                             const int* indices, int num_indices) const {
    return SDL_RenderGeometry(m_renderer, texture, vertices, num_vertices, indices, num_indices);
}

bool Renderer::renderGeometryRaw(SDL_Texture* texture, 
                                const float* xy, int xy_stride,
                                const SDL_FColor* color, int color_stride,
                                const float* uv, int uv_stride,
                                int num_vertices, const void* indices, int num_indices, int size_indices) const {
    return SDL_RenderGeometryRaw(m_renderer, texture, xy, xy_stride, color, color_stride, uv, uv_stride, 
                                num_vertices, indices, num_indices, size_indices);
}

SDL_Surface* Renderer::renderReadPixels(const SDL_Rect* rect) const {
    return SDL_RenderReadPixels(m_renderer, rect);
}

bool Renderer::renderPresent() const {
    return SDL_RenderPresent(m_renderer);
}

void Renderer::destroyRenderer() {
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
}

void Renderer::destroyTexture(SDL_Texture* texture) const {
    SDL_DestroyTexture(texture);
}

bool Renderer::flushRenderer() const {
    return SDL_FlushRenderer(m_renderer);
}

void* Renderer::getRenderMetalLayer() const {
    return SDL_GetRenderMetalLayer(m_renderer);
}

void* Renderer::getRenderMetalCommandEncoder() const {
    return SDL_GetRenderMetalCommandEncoder(m_renderer);
}

bool Renderer::addVulkanRenderSemaphores(Uint32 wait_stage_mask, Sint64 wait_semaphore, Sint64 signal_semaphore) const {
    return SDL_AddVulkanRenderSemaphores(m_renderer, wait_stage_mask, wait_semaphore, signal_semaphore);
}

bool Renderer::setRenderVSync(int vsync) const {
    return SDL_SetRenderVSync(m_renderer, vsync);
}

bool Renderer::getRenderVSync(int* vsync) const {
    return SDL_GetRenderVSync(m_renderer, vsync);
}

bool Renderer::renderDebugText(float x, float y, const char* str) const {
    return SDL_RenderDebugText(m_renderer, x, y, str);
}

bool Renderer::renderDebugTextFormat(float x, float y, SDL_PRINTF_FORMAT_STRING const char* fmt, ...) const SDL_PRINTF_VARARG_FUNC(4) {
    va_list args;
    va_start(args, fmt);
    bool result = SDL_RenderDebugTextFormat(m_renderer, x, y, fmt, args);
    va_end(args);
    return result;
}

} // namespace engine