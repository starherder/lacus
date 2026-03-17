#pragma once

#include "engine/config.h"
#include "game/logic/game_play.h"

namespace game
{
	using namespace engine;

	struct MapConfig
	{
		std::string id;
		std::string name;
		std::string desc;
		int level;
		GamePlayType play_type;
		std::string map_path;
	};
		
	class SceneConfig final
	{
	public:
		SceneConfig() = default;
		~SceneConfig() = default;
		
		SceneConfig(SceneConfig&&) = delete;
		SceneConfig(const SceneConfig&) = delete;
		SceneConfig& operator=(SceneConfig&&) = delete;
		SceneConfig& operator=(const SceneConfig&) = delete;
		
	public:
		bool load(const fs::path& filepath);

		MapConfig* getMapConfig(const std::string& name);

	private:
		JsonConfig _jsonConfig;

		std::map<std::string, MapConfig> _mapConfigs;

		static GamePlayType getGamePlayType(const std::string& name);
	};

}