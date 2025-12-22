#include "color.h"
#include <algorithm>
#include <cctype>

namespace engine
{

const Color Color::White(255, 255, 255);
const Color Color::Black(0, 0, 0);
const Color Color::Dark(64, 64, 64);
const Color Color::Gray(128, 128, 128);
const Color Color::Light(192, 192, 192);
const Color Color::Red(255, 0, 0);
const Color Color::LightRed(255, 128, 128);
const Color Color::DarkRed(128, 0, 0);
const Color Color::Green(0, 255, 0);
const Color Color::LightGreen(128, 255, 128);
const Color Color::DarkGreen(0, 128, 0);
const Color Color::Blue(0, 0, 255);
const Color Color::LightBlue(128, 128, 255);
const Color Color::DarkBlue(0, 0, 128);
const Color Color::Yellow(255, 255, 0);
const Color Color::LightYellow(255, 255, 128);
const Color Color::DarkYellow(128, 128, 0);
const Color Color::Cyan(0, 255, 255);
const Color Color::LightCyan(128, 255, 255);
const Color Color::DarkCyan(0, 128, 128);
const Color Color::Magenta(255, 0, 255);
const Color Color::LightMagenta(255, 128, 255);
const Color Color::DarkMagenta(128, 0, 128);
const Color Color::Invalid(0, 0, 0, 0);

Color::Color(int red, int green, int blue, int alpha)
    : r(red), g(green), b(blue), a(alpha) {}

Color::Color(float red, float green, float blue, float alpha)
    : r(static_cast<uint8_t>(std::clamp(red * 255.0f, 0.0f, 255.0f))),
      g(static_cast<uint8_t>(std::clamp(green * 255.0f, 0.0f, 255.0f))),
      b(static_cast<uint8_t>(std::clamp(blue * 255.0f, 0.0f, 255.0f))),
      a(static_cast<uint8_t>(std::clamp(alpha * 255.0f, 0.0f, 255.0f))) {}

Color::Color(uint32_t rgba) {
    r = (rgba >> 24) & 0xFF;
    g = (rgba >> 16) & 0xFF;
    b = (rgba >> 8) & 0xFF;
    a = rgba & 0xFF;
}

Color::Color(const FColor& fcolor) {
    r = static_cast<uint8_t>(std::clamp(fcolor.r * 255.0f, 0.0f, 255.0f));
    g = static_cast<uint8_t>(std::clamp(fcolor.g * 255.0f, 0.0f, 255.0f));
    b = static_cast<uint8_t>(std::clamp(fcolor.b * 255.0f, 0.0f, 255.0f));
    a = static_cast<uint8_t>(std::clamp(fcolor.a * 255.0f, 0.0f, 255.0f));
}

Color::Color(const HSVColor& hsv) {
    fromHSV(hsv);
}

Color::Color(const SDL_Color& sdlColor) {
    r = sdlColor.r;
    g = sdlColor.g;
    b = sdlColor.b;
    a = sdlColor.a;
}

Color::Color(const SDL_FColor& sdlFColor) {
    r = static_cast<uint8_t>(std::clamp(sdlFColor.r * 255.0f, 0.0f, 255.0f));
    g = static_cast<uint8_t>(std::clamp(sdlFColor.g * 255.0f, 0.0f, 255.0f));
    b = static_cast<uint8_t>(std::clamp(sdlFColor.b * 255.0f, 0.0f, 255.0f));
    a = static_cast<uint8_t>(std::clamp(sdlFColor.a * 255.0f, 0.0f, 255.0f));
}

bool Color::operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

bool Color::operator!=(const Color& other) const {
    return !(*this == other);
}

Color::operator FColor() const {
    return toFColor();
}

Color::operator HSVColor() const {
    return toHSV();
}

Color::operator SDL_Color() const {
    SDL_Color sdlColor;
    sdlColor.r = r;
    sdlColor.g = g;
    sdlColor.b = b;
    sdlColor.a = a;
    return sdlColor;
}

Color::operator SDL_FColor() const {
    SDL_FColor sdlFColor;
    sdlFColor.r = r / 255.0f;
    sdlFColor.g = g / 255.0f;
    sdlFColor.b = b / 255.0f;
    sdlFColor.a = a / 255.0f;
    return sdlFColor;
}

Color::operator uint32_t() const {
    return toUint32();
}

FColor Color::toFColor() const
{
    FColor fcolor;
    fcolor.r = r / 255.0f;
    fcolor.g = g / 255.0f;
    fcolor.b = b / 255.0f;
    fcolor.a = a / 255.0f;
    return fcolor;
}

void Color::fromFColor(const FColor& fcolor)
{
    r = static_cast<uint8_t>(std::clamp(fcolor.r * 255.0f, 0.0f, 255.0f));
    g = static_cast<uint8_t>(std::clamp(fcolor.g * 255.0f, 0.0f, 255.0f));
    b = static_cast<uint8_t>(std::clamp(fcolor.b * 255.0f, 0.0f, 255.0f));
    a = static_cast<uint8_t>(std::clamp(fcolor.a * 255.0f, 0.0f, 255.0f));
}

uint32_t Color::toUint32() const
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void Color::fromUint32(uint32_t ul)
{
    r = (ul >> 24) & 0xFF;
    g = (ul >> 16) & 0xFF;
    b = (ul >> 8) & 0xFF;
    a = ul & 0xFF;
}

HSVColor Color::toHSV() const
{
    HSVColor hsv;
    hsv.a = a / 255.0f;
    
    float r = this->r / 255.0f;
    float g = this->g / 255.0f;
    float b = this->b / 255.0f;
    
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;
    
    hsv.v = max;
    
    if (delta < 1e-5f) {
        hsv.s = 0.0f;
        hsv.h = 0.0f;
        return hsv;
    }
    
    hsv.s = delta / max;
    
    if (max == r) {
        hsv.h = 60.0f * (g - b) / delta;
    } else if (max == g) {
        hsv.h = 60.0f * (b - r) / delta + 120.0f;
    } else {
        hsv.h = 60.0f * (r - g) / delta + 240.0f;
    }
    
    if (hsv.h < 0.0f) {
        hsv.h += 360.0f;
    }
    if (hsv.h >= 360.0f) {
        hsv.h -= 360.0f;
    }
    
    return hsv;
}

void Color::fromHSV(const HSVColor& hsv)
{
    a = static_cast<uint8_t>(hsv.a * 255.0f);
    
    if (hsv.s <= 1e-5f) {
        uint8_t value = static_cast<uint8_t>(hsv.v * 255.0f);
        r = value;
        g = value;
        b = value;
        return;
    }
    
    float h = hsv.h;
    if (h >= 360.0f) h = 0.0f;
    h /= 60.0f;
    
    int sector = static_cast<int>(h);
    float fraction = h - sector;
    
    float p = hsv.v * (1.0f - hsv.s);
    float q = hsv.v * (1.0f - hsv.s * fraction);
    float t = hsv.v * (1.0f - hsv.s * (1.0f - fraction));
    
    float rf, gf, bf;
    
    switch (sector) {
        case 0:
            rf = hsv.v; gf = t; bf = p;
            break;
        case 1:
            rf = q; gf = hsv.v; bf = p;
            break;
        case 2:
            rf = p; gf = hsv.v; bf = t;
            break;
        case 3:
            rf = p; gf = q; bf = hsv.v;
            break;
        case 4:
            rf = t; gf = p; bf = hsv.v;
            break;
        default:
            rf = hsv.v; gf = p; bf = q;
            break;
    }
    
    r = static_cast<uint8_t>(std::clamp(rf * 255.0f, 0.0f, 255.0f));
    g = static_cast<uint8_t>(std::clamp(gf * 255.0f, 0.0f, 255.0f));
    b = static_cast<uint8_t>(std::clamp(bf * 255.0f, 0.0f, 255.0f));
}

void Color::fromHexString(const std::string& str)
{
    if (str.empty() || str[0] != '#' || str.length() < 7) {
        *this = Color::Invalid;
        return;
    }
    
    std::string hex = str.substr(1);
    if (hex.length() == 6) {
        hex += "ff"; // 默认不透明
    }
    
    if (hex.length() != 8) {
        *this = Color::Invalid;
        return;
    }
    
    for (char c : hex) {
        if (!std::isxdigit(c)) {
            *this = Color::Invalid;
            return;
        }
    }
    
    uint32_t rgba = std::stoul(hex, nullptr, 16);
    fromUint32(rgba);
}

std::string Color::toHexString() const
{
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "#%02x%02x%02x%02x", r, g, b, a);
    return std::string(buffer);
}

bool Color::isValid() const
{
    return !(r==0 && g==0 && b==0 && a==0);
}

}