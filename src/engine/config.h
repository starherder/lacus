#pragma once

#include <mutex>
#include <string>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>

#include "engine_wrapper.h"

namespace engine {

class JsonConfig
{
public:
    JsonConfig();
    ~JsonConfig();

    bool load(const fs::path& filepath);
    bool save();

    auto& getJson() { return *_json; }

private:
    std::unique_ptr<nlohmann::json> _json = nullptr;
    fs::path _filepath;
};

class SystemConfig
{
public:
    struct windowCfg
	{
        std::string title;
        int width = 1280;
        int height = 720;
        int fps = 60;
    }window;

    struct logCfg
	{
        int level = 0;
        std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v";
    }log;

    struct resCfg
	{
        fs::path path;
    }res;

public:    
    SystemConfig() = default;
    ~SystemConfig() = default;

    bool load(const fs::path& filepath);
    bool save();

private:
    JsonConfig _json_config;
};

} // namespace engine