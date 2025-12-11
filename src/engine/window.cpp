#include "window.h"

namespace engine {

Window::Window()  
{
}

Window::~Window() 
{
    destroyWindow();
}

// Window creation and destruction
bool Window::createWindow(const char* title, int w, int h, WindowFlags flags) {
    _window = SDL_CreateWindow(title, w, h, (Uint64)(flags));
    return _window != nullptr;
}

void Window::destroyWindow() {
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

// Window size
bool Window::getWindowSize(int* w, int* h) {
    if (!_window) return false;
    SDL_GetWindowSize(_window, w, h);
    return true;
}

bool Window::setWindowSize(int w, int h) {
    if (!_window) return false;
    SDL_SetWindowSize(_window, w, h);
    return true;
}

bool Window::getWindowSizeInPixels(int* w, int* h) {
    if (!_window) return false;
    SDL_GetWindowSizeInPixels(_window, w, h);
    return true;
}

// Window position
bool Window::getWindowPosition(int* x, int* y) {
    if (!_window) return false;
    SDL_GetWindowPosition(_window, x, y);
    return true;
}

bool Window::setWindowPosition(int x, int y) {
    if (!_window) return false;
    SDL_SetWindowPosition(_window, x, y);
    return true;
}

// Window title
bool Window::setWindowTitle(const char* title) {
    if (!_window) return false;
    SDL_SetWindowTitle(_window, title);
    return true;
}

const char* Window::getWindowTitle() {
    if (!_window) return nullptr;
    return SDL_GetWindowTitle(_window);
}

// Window visibility and state
bool Window::showWindow() {
    if (!_window) return false;
    SDL_ShowWindow(_window);
    return true;
}

bool Window::hideWindow() {
    if (!_window) return false;
    SDL_HideWindow(_window);
    return true;
}

bool Window::raiseWindow() {
    if (!_window) return false;
    SDL_RaiseWindow(_window);
    return true;
}

bool Window::maximizeWindow() {
    if (!_window) return false;
    SDL_MaximizeWindow(_window);
    return true;
}

bool Window::minimizeWindow() {
    if (!_window) return false;
    SDL_MinimizeWindow(_window);
    return true;
}

bool Window::restoreWindow() {
    if (!_window) return false;
    SDL_RestoreWindow(_window);
    return true;
}

bool Window::setWindowFullscreen(bool fullscreen) {
    if (!_window) return false;
    SDL_SetWindowFullscreen(_window, fullscreen);
    return true;
}

const SDL_DisplayMode* Window::getWindowFullscreenMode() {
    if (!_window) return nullptr;
    return SDL_GetWindowFullscreenMode(_window);
}

bool Window::setWindowFullscreenMode(const SDL_DisplayMode* mode) {
    if (!_window) return false;
    SDL_SetWindowFullscreenMode(_window, mode);
    return true;
}

// Window properties
WindowFlags Window::getWindowFlags() {
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

bool Window::getDisplayBounds(SDL_DisplayID displayID, SDL_Rect* rect) {
    return SDL_GetDisplayBounds(displayID, rect);
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