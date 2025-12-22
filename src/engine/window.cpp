#include "window.h"

namespace engine {

Window::Window()  
{
}

Window::~Window()
{
    destroy();
}

// Window creation and destruction
bool Window::create(const char* title, int w, int h, WindowFlags flags) {
    _window = SDL_CreateWindow(title, w, h, (Uint64)(flags));
    return _window != nullptr;
}

void Window::destroy() {
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

// Window size
Vec2i Window::getSize() const {
    assert(_window);

    int w, h;
    SDL_GetWindowSize(_window, &w, &h);
    return {w, h};
}

void Window::setSize(const Vec2i& size) {
    assert(_window);

    SDL_SetWindowSize(_window, size.x, size.y);
}

Vec2i Window::getSizeInPixels() const {
    assert(_window);

    int w, h;
    SDL_GetWindowSizeInPixels(_window, &w, &h);
    return {w, h};
}

Vec2i Window::getPosition() const {
    assert(_window);

    int x, y;
    SDL_GetWindowPosition(_window, &x, &y);
    return {x, y};
}

void Window::setPosition(const Vec2i& pos) {
    assert(_window);

    SDL_SetWindowPosition(_window, pos.x, pos.y);
}

// Window title
bool Window::setTitle(const char* title) {
    if (!_window) return false;
    SDL_SetWindowTitle(_window, title);
    return true;
}

const char* Window::getTitle() const {
    if (!_window) return nullptr;
    return SDL_GetWindowTitle(_window);
}

// Window visibility and state
bool Window::show() {
    if (!_window) return false;
    SDL_ShowWindow(_window);
    return true;
}

bool Window::hide() {
    if (!_window) return false;
    SDL_HideWindow(_window);
    return true;
}

bool Window::raise() {
    if (!_window) return false;
    SDL_RaiseWindow(_window);
    return true;
}

bool Window::maximize() {
    if (!_window) return false;
    SDL_MaximizeWindow(_window);
    return true;
}

bool Window::minimize() {
    if (!_window) return false;
    SDL_MinimizeWindow(_window);
    return true;
}

bool Window::restore() {
    if (!_window) return false;
    SDL_RestoreWindow(_window);
    return true;
}

bool Window::setFullscreen(bool fullscreen) {
    if (!_window) return false;
    SDL_SetWindowFullscreen(_window, fullscreen);
    return true;
}

const SDL_DisplayMode* Window::getFullscreenMode() {
    if (!_window) return nullptr;
    return SDL_GetWindowFullscreenMode(_window);
}

bool Window::setFullscreenMode(const SDL_DisplayMode* mode) {
    if (!_window) return false;
    SDL_SetWindowFullscreenMode(_window, mode);
    return true;
}

// Window properties
WindowFlags Window::getFlags() {
    if (!_window) return WindowFlags::Hidden; // Return a default flag when no _window exists
    return static_cast<WindowFlags>(SDL_GetWindowFlags(_window));
}

// Display functions
SDL_DisplayID* Window::getDisplays(int* count) {
    return SDL_GetDisplays(count);
}

const char* Window::getDisplayName(SDL_DisplayID displayID) {
    return SDL_GetDisplayName(displayID);
}

Rect Window::getDisplayBounds(SDL_DisplayID displayID) {
    SDL_Rect sdlRect;
    SDL_GetDisplayBounds(displayID, &sdlRect);
    return sdlRect;
}

// Screen saver
bool Window::screenSaverEnabled() {
    return SDL_ScreenSaverEnabled();
}

bool Window::enableScreenSaver() {
    return SDL_EnableScreenSaver();
}

bool Window::disableScreenSaver() {
    return SDL_DisableScreenSaver();
}

} // namespace engine