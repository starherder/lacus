#pragma once

#include "wrapper.h"

#include "spdlog/spdlog.h"
#include "config.h"
#include <optional>

namespace engine
{

class IResManager 
{
public:
    const fs::path& resPath() const { return _resPath; }
    virtual void setResPath(const fs::path& path) { _resPath = path; }

private:
    fs::path _resPath;
};


struct FpsChecker
{
public:
    void init(int fixed_fps);

    bool check();

    float deltaSeconds() const { return _deltaTicks/1000.0f; }

    int deltaTicks() const { return _deltaTicks; }

    int fps() const { return _curFps; }

    int fixedFps() const { return _fixedFps; }

    int avgFps() const { return _avgFps; };

private:
    uint64_t _lastTicks = 0;
    uint64_t _frameTicks = 0;

    uint32_t _deltaTicks = 0;
    
    int _curFps = 0;
    int _avgFps = 0;
    int _fixedFps = 60;

    int _lastSecond = 0;

    std::vector<int> _curSecondFps;
};

class PathUtils 
{
public:    

    static fs::path get_executable_path();

    static bool is_path_inside_directory(const fs::path& path, const fs::path& base_dir);
    
    static std::optional<fs::path> get_relative_path(const fs::path& path, const fs::path& base);
    
    static uintmax_t calculate_directory_size(const fs::path& dir);
};

struct HSVColor
{
    float h; // 色相 [0,360]
    float s; // 饱和度 [0,1]
    float v; // 亮度  [0,1]
    float a; // 透明度 [0,1]  不变
};

class ColorUtils
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

    static FColor to_fcolor(const Color& color);
    static Color to_color(const FColor& fcolor);

    static FColor to_fcolor(uint32_t ul);
    static Color to_color(uint32_t ul);

    static uint32_t to_uint32(const Color& color);
    static uint32_t to_uint32(const FColor& fcolor);

    static HSVColor FRGB_to_HSV(const FColor& fcolor);
    static HSVColor RGB_to_HSV(const Color& color);

    static Color HSV_to_RGB(const HSVColor& hsv);
    static FColor HSV_to_FRGB(const HSVColor& hsv);

    // hex number start with '#' like "#ff1c4dd4"
    static Color from_shap_string(const std::string& str);
    static std::string to_shap_string(const Color& color);

    static bool is_valid(const Color& color);
};

}