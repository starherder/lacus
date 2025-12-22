#include "misc.h"


namespace engine {


void FpsChecker::init(int fixed_fps)
{
    if(fixed_fps > 0) {
        _fixedFps = fixed_fps;
    }

    _frameTicks = (uint64_t)(1000.0f / _fixedFps);
    _lastTicks = SDL_GetTicks();
    _lastSecond = (int)_lastTicks / 1000;
    _deltaTicks = 0;
}

bool FpsChecker::check()
{
    uint64_t current_ticks = SDL_GetTicks();
    if (current_ticks - _lastTicks < _frameTicks)
    {
        return false;
    }

    _deltaTicks = (int)(current_ticks - _lastTicks);
    _lastTicks = current_ticks;

    _curFps = int(1000.0f /_deltaTicks);

    int cur_second = (int)current_ticks / 1000;

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

}