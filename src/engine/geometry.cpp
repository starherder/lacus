#include "geometry.h"

namespace engine {

Rect::Rect(int x_, int y_, int w_, int h_) : x(float(x_)), y(float(y_)), w(float(w_)), h(float(h_)) {}

Rect::Rect(float x_, float y_, float w_, float h_) : x(x_), y(y_), w(w_), h(h_) {}

Rect::Rect(const SDL_FRect& frect) : x(frect.x), y(frect.y), w(frect.w), h(frect.h) {}

Rect::Rect(const SDL_Rect& rect) : x(float(rect.x)), y(float(rect.y)), w(float(rect.w)), h(float(rect.h)) {}

Rect::Rect(const Vec2& pos, const Vec2& size)
{
    x = pos.x;
    y = pos.y;
    w = size.x;
    h = size.y;
}

Vec2 Rect::pos() { 
    return Vec2{x, y}; 
}

Vec2 Rect::size() { 
    return Vec2{w, h}; 
}

bool Rect::contains(const Vec2& point) const {
    return point.x >= x && point.x < x + w && point.y >= y && point.y < y + h;
}

Rect::operator SDL_FRect() const {
    SDL_FRect frect;
    frect.x = x;
    frect.y = y;
    frect.w = w;
    frect.h = h;
    return frect;
}

Rect::operator SDL_Rect() const {
    SDL_Rect rect;
    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);
    rect.w = static_cast<int>(w);
    rect.h = static_cast<int>(h);
    return rect;
}

bool Rect::operator==(const Rect& other) const {
    return x == other.x && y == other.y && w == other.w && h == other.h;
}

bool Rect::operator!=(const Rect& other) const {
    return !(*this == other);
}

}