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


struct FrameTicker
{
public:
    void init(int fixed_fps);

    bool check();

    float deltaSeconds() const { return _deltaTicks/1000.0f; }

    int deltaTicks() const { return _deltaTicks; }

    uint64_t ticks() const;

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


}

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" 
#include "spdlog/sinks/stdout_sinks.h"

inline std::ostream& operator<<(std::ostream& os, const engine::Vec2& v)
{
    return os << "(" << v.x << "," << v.y << ")";
}
template <> struct fmt::formatter<engine::Vec2> : fmt::ostream_formatter {};

inline std::ostream& operator<<(std::ostream& os, const engine::Vec2i& v)
{
    return os << "(" << v.x << "," << v.y << ")";
}
template <> struct fmt::formatter<engine::Vec2i> : fmt::ostream_formatter {};

inline std::ostream& operator<<(std::ostream& os, const engine::Rect& r)
{
    return os << "(" << r.x << "," << r.y << r.w << "," << r.h << ")";
}
template <> struct fmt::formatter<engine::Rect> : fmt::ostream_formatter {};

inline std::ostream& operator<<(std::ostream& os, const engine::Color& c)
{
    return os << "(" << (int)c.r << "," << (int)c.b << (int)c.b << "," << (int)c.a << ")";
}
template <> struct fmt::formatter<engine::Color> : fmt::ostream_formatter {};

inline std::ostream& operator<<(std::ostream& os, const entt::entity& ent)
{
    return os << (uint32_t)ent;
}
template <> struct fmt::formatter<entt::entity> : fmt::ostream_formatter {};

inline std::ostream& operator<<(std::ostream& os, const std::filesystem::path& path)
{
    return os << path.string();
}
template <> struct fmt::formatter<std::filesystem::path> : fmt::ostream_formatter {};
