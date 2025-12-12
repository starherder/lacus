#pragma once

#include "wrapper.h"

#include "spdlog/spdlog.h"
#include "config.h"
#include <optional>

namespace engine
{

struct FpsChecker
{
public:
    void init(int fixed_fps);

    bool check();

    float delta_time() const { return _delta_time; }
    
    int fps() const { return _cur_fps; }

    int fixed_fps() const { return _fixed_fps; }

    int avg_fps() const { return _avg_fps; };

private:
    uint64_t _last_ticks = 0;
    uint64_t _frame_ticks = 0;
    float _delta_time = 0.0f;

    int _cur_fps = 0;
    int _avg_fps = 0;
    int _fixed_fps = 60;

    int _last_second = 0;

    std::vector<int> _cur_second_fps;
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
    static FColor to_fcolor(const Color& color);
    static Color to_color(const FColor& fcolor);

    static uint32_t to_uint32(const Color& color);
    static uint32_t to_uint32(const FColor& fcolor);

    static HSVColor FRGB_to_HSV(const FColor& fcolor);
    static HSVColor RGB_to_HSV(const Color& color);

    static Color HSV_to_RGB(const HSVColor& hsv);
    static FColor HSV_to_FRGB(const HSVColor& hsv);
};

}