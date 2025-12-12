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
    bool createWindow(const char* title, int w, int h, WindowFlags flags);
    
    // Window size
    Vec2i getWindowSize() const;
    void setWindowSize(const Vec2i& size);
    Vec2i getWindowSizeInPixels() const;
    
    // Window position
    Vec2i getWindowPosition() const;
    void setWindowPosition(const Vec2i& pos);
    
    // Window title
    bool setWindowTitle(const char* title);
    const char* getWindowTitle() const;
    
    // Window visibility and state
    bool showWindow();
    bool hideWindow();
    bool raiseWindow();
    bool maximizeWindow();
    bool minimizeWindow();
    bool restoreWindow();
    bool setWindowFullscreen(bool fullscreen);
    const SDL_DisplayMode* getWindowFullscreenMode();
    bool setWindowFullscreenMode(const SDL_DisplayMode* mode);
    
    // Window properties
    WindowFlags getWindowFlags();
    SDL_Window* getSdlWindow() { return _window; }
    
    // Display functions
    static SDL_DisplayID* getDisplays(int* count);
    static const char* getDisplayName(SDL_DisplayID displayID);
    static bool getDisplayBounds(SDL_DisplayID displayID, SDL_Rect* rect);
    
    // Screen saver
    static bool screenSaverEnabled();
    static bool enableScreenSaver();
    static bool disableScreenSaver();

private:
    void destroyWindow();

private:
    SDL_Window* _window = nullptr;
};

} // namespace engine