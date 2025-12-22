#pragma once

#include <string>
#include <cstdint>

#include <SDL3/SDL.h>

namespace engine
{

struct HSVColor
{
    float h; // 色相 [0,360]
    float s; // 饱和度 [0,1]
    float v; // 亮度  [0,1]
    float a; // 透明度 [0,1] 
};

struct FColor
{
    float r; // 0.0f ~ 1.0f
    float g; // 0.0f ~ 1.0f
    float b; // 0.0f ~ 1.0f
    float a; // 0.0f ~ 1.0f
};

class Color
{
public:    
    static const Color White;
    static const Color Black;
    static const Color Dark;
    static const Color Gray;
    static const Color Light;
    static const Color Red;
    static const Color LightRed;
    static const Color DarkRed;
    static const Color Green;
    static const Color LightGreen;
    static const Color DarkGreen;
    static const Color Blue;
    static const Color LightBlue;
    static const Color DarkBlue;
    static const Color Yellow;
    static const Color LightYellow;
    static const Color DarkYellow;
    static const Color Cyan;
    static const Color LightCyan;
    static const Color DarkCyan;
    static const Color Magenta;
    static const Color LightMagenta;
    static const Color DarkMagenta;
    static const Color Invalid;

public:
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

    Color() = default;
    Color(int red, int green, int blue, int alpha = 255);
    Color(float red, float green, float blue, float alpha = 1.0f);

    Color(uint32_t rgba);
    Color(const FColor& fcolor);
    Color(const HSVColor& hsv);
    Color(const SDL_Color& sdlColor);
    Color(const SDL_FColor& sdlFColor);

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;

    operator uint32_t() const;
    operator FColor() const;
    operator HSVColor() const;
    operator SDL_Color() const;
    operator SDL_FColor() const;

    bool isValid() const;
  
    FColor toFColor() const;
    void fromFColor(const FColor& fcolor);

    uint32_t toUint32() const;
    void fromUint32(uint32_t ul);

    HSVColor toHSV() const;
    void fromHSV(const HSVColor& hsv);

    std::string toHexString() const;
    void fromHexString(const std::string& str);
};

}