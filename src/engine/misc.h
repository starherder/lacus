#pragma once

#include "wrapper.h"

#include "spdlog/spdlog.h"
#include "config.h"
#include <optional>

namespace engine
{

struct FrameChecker
{
public:
    void init(int fixed_fps);

    bool check();

    float delta_time();

private:
    uint64_t _last_ticks = 0;
    uint64_t _frame_ticks = 0;
    float _delta_time = 0.0f;
};

class PathUtils 
{
public:    

    static fs::path get_executable_path();

    static bool is_path_inside_directory(const fs::path& path, const fs::path& base_dir);
    
    static std::optional<fs::path> get_relative_path(const fs::path& path, const fs::path& base);
    
    static uintmax_t calculate_directory_size(const fs::path& dir);
};

}