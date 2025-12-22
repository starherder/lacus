#pragma once

#include "wrapper.h"

namespace engine {

class Window final
{
public:
    Window();
    ~Window();

    Window(const Window& wnd) = delete;
    Window& operator=(const Window& wnd) = delete;

    // Window creation and destruction
    bool create(const char* title, int w, int h, WindowFlags flags);

    // Window size
    Vec2i getSize() const;
    void setSize(const Vec2i& size);
    Vec2i getSizeInPixels() const;

    // Window position
    Vec2i getPosition() const;
    void setPosition(const Vec2i& pos);

    // Window title
    bool setTitle(const char* title);
    const char* getTitle() const;

    // Window visibility and state
    bool show();
    bool hide();
    bool raise();
    bool maximize();
    bool minimize();
    bool restore();
    bool setFullscreen(bool fullscreen);
    const SDL_DisplayMode* getFullscreenMode();
    bool setFullscreenMode(const SDL_DisplayMode* mode);

    // Window properties
    WindowFlags getFlags();
    SDL_Window* getSdlWindow() { return _window; }
    
    // Display functions
    static SDL_DisplayID* getDisplays(int* count);
    static const char* getDisplayName(SDL_DisplayID displayID);
    static Rect getDisplayBounds(SDL_DisplayID displayID);
    
    // Screen saver
    static bool screenSaverEnabled();
    static bool enableScreenSaver();
    static bool disableScreenSaver();

private:
    void destroy();

private:
    SDL_Window* _window = nullptr;
};

} // namespace engine