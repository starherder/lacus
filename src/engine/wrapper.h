#pragma once

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory.h>
#include <unordered_map>
#include <set>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace engine {

namespace fs = std::filesystem;

using Event = SDL_Event;

using Color = SDL_Color;
using FColor = SDL_FColor;

using Rect = SDL_Rect;
using FRect = SDL_FRect;

using Vec2f = glm::fvec2;
using Vec3f = glm::fvec3;
using Vec4f = glm::fvec4;

using Vec2i = glm::ivec2;
using Vec3i = glm::ivec3;
using Vec4i = glm::ivec4;

using FlipMode = SDL_FlipMode;

enum class WindowEventType 
{
    Shown = SDL_EVENT_WINDOW_SHOWN,
    Hidden = SDL_EVENT_WINDOW_HIDDEN,
    Exposed = SDL_EVENT_WINDOW_EXPOSED,
    Moved = SDL_EVENT_WINDOW_MOVED,
    Resized = SDL_EVENT_WINDOW_RESIZED,
    PixelSizeChanged = SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED,
    MetalViewResized = SDL_EVENT_WINDOW_METAL_VIEW_RESIZED,
    Minimized = SDL_EVENT_WINDOW_MINIMIZED,
    Maximized = SDL_EVENT_WINDOW_MAXIMIZED,
    Restored = SDL_EVENT_WINDOW_RESTORED,
    MouseEnter = SDL_EVENT_WINDOW_MOUSE_ENTER,
    MouseLeave = SDL_EVENT_WINDOW_MOUSE_LEAVE,
    FocusGained = SDL_EVENT_WINDOW_FOCUS_GAINED,
    FocusLost = SDL_EVENT_WINDOW_FOCUS_LOST,
    CloseRequested = SDL_EVENT_WINDOW_CLOSE_REQUESTED,
    HitTest = SDL_EVENT_WINDOW_HIT_TEST,
    ICCProfChanged = SDL_EVENT_WINDOW_ICCPROF_CHANGED,
    DisplayChanged = SDL_EVENT_WINDOW_DISPLAY_CHANGED,
    DisplayScaleChanged = SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED,
    SafeAreaChanged = SDL_EVENT_WINDOW_SAFE_AREA_CHANGED,
    Occcluded = SDL_EVENT_WINDOW_OCCLUDED,
    EnterFullscreen = SDL_EVENT_WINDOW_ENTER_FULLSCREEN,
    LeaveFullscreen = SDL_EVENT_WINDOW_LEAVE_FULLSCREEN,
    Destroyed = SDL_EVENT_WINDOW_DESTROYED,
    HDRStateChanged = SDL_EVENT_WINDOW_HDR_STATE_CHANGED
};

enum class WindowFlags : Uint64 
{
    Fullscreen = SDL_WINDOW_FULLSCREEN,
    OpenGL = SDL_WINDOW_OPENGL,
    Occcluded = SDL_WINDOW_OCCLUDED,
    Hidden = SDL_WINDOW_HIDDEN,
    Borderless = SDL_WINDOW_BORDERLESS,
    Resizable = SDL_WINDOW_RESIZABLE,
    Minimized = SDL_WINDOW_MINIMIZED,
    Maximized = SDL_WINDOW_MAXIMIZED,
    MouseGrabbed = SDL_WINDOW_MOUSE_GRABBED,
    InputFocus = SDL_WINDOW_INPUT_FOCUS,
    MOUSE_FOCUS = SDL_WINDOW_MOUSE_FOCUS,
    External = SDL_WINDOW_EXTERNAL,
    Modal = SDL_WINDOW_MODAL,
    HighPixelDensity = SDL_WINDOW_HIGH_PIXEL_DENSITY,
    MouseCapture = SDL_WINDOW_MOUSE_CAPTURE,
    MouseRelativeMode = SDL_WINDOW_MOUSE_RELATIVE_MODE,
    AlwaysOnTop = SDL_WINDOW_ALWAYS_ON_TOP,
    Utility = SDL_WINDOW_UTILITY,
    Tooltip = SDL_WINDOW_TOOLTIP,
    PopupMenu = SDL_WINDOW_POPUP_MENU,
    KeyboardGrabbed = SDL_WINDOW_KEYBOARD_GRABBED,
    Vulkan = SDL_WINDOW_VULKAN,
    Metal = SDL_WINDOW_METAL,
    Transparent = SDL_WINDOW_TRANSPARENT,
    NotFocusable = SDL_WINDOW_NOT_FOCUSABLE
};


enum class BlendMode : Uint32
{
    None = SDL_BLENDMODE_NONE,
    Blend = SDL_BLENDMODE_BLEND,
    BlendPremultiplied = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    Add = SDL_BLENDMODE_ADD,
    AddPremultiplied = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    Mod = SDL_BLENDMODE_MOD,
    Mul = SDL_BLENDMODE_MUL,
    Invalid = SDL_BLENDMODE_INVALID
};

}