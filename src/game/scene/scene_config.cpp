#include "scene_config.h"

#include "engine/wrapper.h"
#include "nlohmann/json.hpp"
#include "utility/translator.h"

namespace game
{
	MapConfig* SceneConfig::getMapConfig(const std::string& name)
	{
		auto it = _mapConfigs.find(name);
		if (it != _mapConfigs.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	bool SceneConfig::load(const fs::path& filepath)
	{
		bool res = _jsonConfig.load(filepath);
		if (!res) {
			LogError("GameConfig:load {} failed.", filepath.string());
			return false;
		}

		auto& json = _jsonConfig.getJson();
		//dying_ticks = json.value("dying_ticks", 1000);

		if (!json.is_array())
		{
			LogError("GameConfig:load {} failed, not array.", filepath.string());
			return false;
		}

		for (auto& item : json)
		{
			MapConfig config;
			config.id = item.value("id", "");
			config.name = Trans(item.value("name", ""));
			config.desc = Trans(item.value("desc", ""));
			config.level = item.value("level", 1);
			config.play_type = getGamePlayType(item.value("play_type", ""));
			config.map_path = item.value("map_path", "");

			_mapConfigs.insert({config.id, config});
		}
		return true;
	}

	GamePlayType SceneConfig::getGamePlayType(const std::string& name)
	{
		if (name == "TileBattle") return GamePlayType::GamePlay_TileBattle;
		else if (name == "AutoChess") return GamePlayType::GamePlay_AutoChess;
		return GamePlayType::GamePlay_TileBattle;
	}


}