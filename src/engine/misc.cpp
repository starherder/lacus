#include "misc.h"


namespace engine {


void FpsChecker::init(int fixed_fps)
{
    if(fixed_fps > 0) {
        _fixedFps = fixed_fps;
    }

    _frameTicks = (uint64_t)(1000.0f / _fixedFps);
    _lastTicks = SDL_GetTicks();
    _lastSecond = _lastTicks / 1000;
    _deltaTicks = 0;
}

bool FpsChecker::check()
{
    uint64_t current_ticks = SDL_GetTicks();
    if (current_ticks - _lastTicks < _frameTicks)
    {
        return false;
    }

    _deltaTicks = current_ticks - _lastTicks;
    _lastTicks = current_ticks;

    _curFps = int(1000.0f /_deltaTicks);

    int cur_second = current_ticks / 1000;

    if(cur_second != _lastSecond)
    {
        int total = 0;
        int count = (int)_curSecondFps.size();
        for(auto& fps : _curSecondFps) { total += fps;}

        _avgFps = (count==0) ? 0 : (total / count);

        _curSecondFps.clear();
        _lastSecond = cur_second;
    }
    else
    {
        _curSecondFps.push_back(_curFps);
    }

    return true;
}


fs::path PathUtils::get_executable_path() 
{
    #ifdef _WIN32
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        return fs::path(path).parent_path();
    #elif defined(__linux__)
        char path[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
        return fs::path(std::string(path, count > 0 ? count : 0)).parent_path();
    #elif defined(__APPLE__)
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) {
            return fs::path(path).parent_path();
        }
        return fs::current_path(); // 失败时回退到当前目录
    #endif
}

// 检查路径是否在指定目录内（安全限制）
bool PathUtils::is_path_inside_directory(const fs::path& path, const fs::path& base_dir) 
{
    fs::path canonical_path = fs::canonical(path);
    fs::path canonical_base = fs::canonical(base_dir);
    
    auto mismatch_pair = std::mismatch(
        canonical_base.begin(), canonical_base.end(),
        canonical_path.begin()
    );
    
    return mismatch_pair.first == canonical_base.end();
}

// 获取相对路径
std::optional<fs::path> PathUtils::get_relative_path(const fs::path& path, const fs::path& base)
{
    try {
        return fs::relative(path, base);
    } catch (const fs::filesystem_error&) {
        return std::nullopt;
    }
}

// 计算目录大小
uintmax_t PathUtils::calculate_directory_size(const fs::path& dir) 
{
    uintmax_t total_size = 0;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                total_size += entry.file_size();
            }
        }
    } catch (const fs::filesystem_error& e) {
        spdlog::error("calc path size error: {}", e.what());
    }
    
    return total_size;
}


FColor ColorUtils::to_fcolor(const Color& c)
{
    return {c.r/255.0f, c.g/255.0f, c.b/255.0f, c.a/255.0f};
}

Color ColorUtils::to_color(const FColor& c)
{
    return {uint8_t(c.r*255), uint8_t(c.g*255), uint8_t(c.b*255), uint8_t(c.a*255)};
}

uint32_t ColorUtils::to_uint32(const Color& c)
{
    return c.r<<24 | c.g<<16 | c.b<<8 | c.a;
}

uint32_t ColorUtils::to_uint32(const FColor& fc)
{
    return to_uint32(to_color(fc));
}

HSVColor ColorUtils::FRGB_to_HSV(const FColor& rgb)
{
    auto c = to_color(rgb);
    return RGB_to_HSV(c);
}

FColor ColorUtils::HSV_to_FRGB(const HSVColor& hsv)
{
    auto c = HSV_to_RGB(hsv);
    return to_fcolor(c);
}

HSVColor ColorUtils::RGB_to_HSV(const Color& rgb)
{
    HSVColor hsv;
    hsv.a = rgb.a / 255.0f; // 保持Alpha通道不变，转换为[0,1]范围
    
    // 将RGB分量归一化到[0,1]范围
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;
    
    // 计算最大值、最小值和差值
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float delta = max - min;
    
    // 计算亮度（Value）
    hsv.v = max;
    
    // 处理灰度色（饱和度为零的情况）
    if (delta < 1e-5f) {
        hsv.s = 0.0f;
        hsv.h = 0.0f; // 灰度色时色相未定义，设为0
        return hsv;
    }
    
    // 计算饱和度
    hsv.s = delta / max;
    
    // 计算色相[1,7,9](@ref)
    if (max == r) {
        hsv.h = 60.0f * (g - b) / delta;
    } else if (max == g) {
        hsv.h = 60.0f * (b - r) / delta + 120.0f;
    } else { // max == b
        hsv.h = 60.0f * (r - g) / delta + 240.0f;
    }
    
    // 确保色相在[0,360)范围内[1](@ref)
    if (hsv.h < 0.0f) {
        hsv.h += 360.0f;
    }
    if (hsv.h >= 360.0f) {
        hsv.h -= 360.0f;
    }
    
    return hsv;
}

Color ColorUtils::HSV_to_RGB(const HSVColor& hsv)
{
    Color rgb;
    rgb.a = static_cast<Uint8>(hsv.a * 255.0f); // 保持Alpha通道不变
    
    // 处理饱和度为0的情况（灰度色）
    if (hsv.s <= 1e-5f) {
        Uint8 value = static_cast<Uint8>(hsv.v * 255.0f);
        rgb.r = value;
        rgb.g = value;
        rgb.b = value;
        return rgb;
    }
    
    // 规范化色相到[0,360)范围
    float h = hsv.h;
    if (h >= 360.0f) h = 0.0f;
    h /= 60.0f; // 将色相缩放到[0,6)范围
    
    // 计算色相所在的扇形区域和小数部分[1,11](@ref)
    int sector = static_cast<int>(h);
    float fraction = h - sector;
    
    // 计算中间值
    float p = hsv.v * (1.0f - hsv.s);
    float q = hsv.v * (1.0f - hsv.s * fraction);
    float t = hsv.v * (1.0f - hsv.s * (1.0f - fraction));
    
    float r, g, b;
    
    // 根据色相扇形区域计算RGB分量[1](@ref)
    switch (sector) {
        case 0:
            r = hsv.v; g = t; b = p;
            break;
        case 1:
            r = q; g = hsv.v; b = p;
            break;
        case 2:
            r = p; g = hsv.v; b = t;
            break;
        case 3:
            r = p; g = q; b = hsv.v;
            break;
        case 4:
            r = t; g = p; b = hsv.v;
            break;
        default: // case 5
            r = hsv.v; g = p; b = q;
            break;
    }
    
    // 将浮点RGB值转换为8位整数
    rgb.r = static_cast<Uint8>(std::clamp(r * 255.0f, 0.0f, 255.0f));
    rgb.g = static_cast<Uint8>(std::clamp(g * 255.0f, 0.0f, 255.0f));
    rgb.b = static_cast<Uint8>(std::clamp(b * 255.0f, 0.0f, 255.0f));
    
    return rgb; 
}

}