#include "config.h"
#include <fstream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace engine {

JsonConfig::JsonConfig() 
{
    _json = std::make_unique<nlohmann::json>();
}

JsonConfig::~JsonConfig() 
{
}

bool JsonConfig::load(const fs::path& filepath) 
{
    _filepath = filepath.string();

    std::ifstream ifile(_filepath);
    if (!ifile.is_open()) {
        SPDLOG_ERROR("open file '{}' failed.", _filepath.string());
        return false;
    }

    try {
        ifile >> (*_json);

        SPDLOG_INFO("load json from '{}' OK.", _filepath.string());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("load json form '{}' failed, err = '{}'", _filepath.string(), e.what());
        return false;
    }
    
    return true;
}

bool JsonConfig::save() 
{
    std::ofstream ofile(_filepath);
    if (!ofile.is_open()) 
    {
        SPDLOG_ERROR("open file '{}' failed.", _filepath.string());
        return false;
    }
    
    try 
    {
        ofile << (*_json).dump(4);
        SPDLOG_INFO("save json to '{}' OK.", _filepath.string());
    }
    catch (const std::exception& e) 
    {
        SPDLOG_ERROR("save json to '{}' failed, err = '{}'", _filepath.string(), e.what());
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------
bool SystemConfig::load(const fs::path& filepath) 
{
    if(!_json_config.load(filepath))
    {
        return false;
    }

    try {

        auto& json = _json_config.getJson();
        if(json.contains("window"))
        {
            auto& window_json = json["window"];
            window.title = window_json["title"];
            window.width = window_json["width"];
            window.height = window_json["height"];
            window.fps = window_json["fps"];
        }
        else 
        {
            SPDLOG_ERROR("window NOT set.");
            return false;
        }

        if(json.contains("log"))
        {
            auto& log_json = json["log"];
            log.level = log_json["level"];
            log.pattern = log_json["pattern"];
        }

        if(json.contains("res"))
        {
            auto& res_json = json["res"];
            std::string res_path_str = res_json["path"];
            res.path = fs::current_path() / res_path_str;
        }
        else 
        {
            SPDLOG_ERROR("res path NOT set.");
            return false;
        }

        if (json.contains("sound"))
        {
            auto& soundJs = json["sound"];
            sound.sound_volumn = soundJs.value("sound_volumn", 1.0f);
            sound.music_volumn = soundJs.value("music_volumn", 1.0f);
        }

        debug_mode = json.value("debug", false);
    }
    catch(const std::exception& e)
    {
        SPDLOG_ERROR("load system config from '{}' failed, err = '{}'", filepath.string(), e.what());
        return false;
    }

    return true;
}

bool SystemConfig::save() 
{
    auto& json = _json_config.getJson();

    json["window"]["title"] = window.title;
    json["window"]["width"] = window.width;
    json["window"]["height"] = window.height;
    json["window"]["fps"] = window.fps;

    json["log"]["level"] = log.level;
    json["log"]["pattern"] = log.pattern;

    return _json_config.save();
}


} // namespace engine