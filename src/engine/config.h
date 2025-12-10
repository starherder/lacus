#pragma once

#include <mutex>
#include <string>
#include <filesystem>
#include <nlohmann/json_fwd.hpp>

namespace engine {

class JsonConfig
{
public:
    JsonConfig();
    ~JsonConfig();

    bool load(const std::filesystem::path& filepath);
    bool save();

    auto& getJson() { return *_json; }

private:
    std::unique_ptr<nlohmann::json> _json = nullptr;
    std::filesystem::path _filepath;
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
        std::filesystem::path path;
    }res;

    struct ImGuiCfg
	{
        float ui_scale = 1.0f;
        float ui_alpha = 1.0f;
        std::string font_file;
        float font_size = 16.0f;
    } imgui;

public:    
    SystemConfig() = default;
    ~SystemConfig() = default;

    bool load(const std::filesystem::path& filepath);
    bool save();

private:
    JsonConfig _json_config;
};

} // namespace engine