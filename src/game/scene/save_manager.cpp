#include "save_manager.h"

#include "game/scene/game_data.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace game
{
	bool SaveManager::saveGameData(const std::filesystem::path& filepath, const GameData& data)
	{
		if (filepath.has_parent_path())
		{
			std::error_code ec;
			std::filesystem::create_directories(filepath.parent_path(), ec);
			if (ec)
			{
				LogError("create save dir '{}' failed, err = '{}'", filepath.parent_path().string(), ec.message());
				return false;
			}
		}

		std::ofstream ofile(filepath);
		if (!ofile.is_open())
		{
			LogError("open file '{}' failed.", filepath.string());
			return false;
		}

		try
		{
			ofile << data.toJson().dump(4);
			LogInfo("save game data to '{}' OK.", filepath.string());
		}
		catch (const std::exception& e)
		{
			LogError("save game data to '{}' failed, err = '{}'", filepath.string(), e.what());
			return false;
		}

		return true;
	}

	bool SaveManager::loadGameData(const std::filesystem::path& filepath, GameData& data)
	{
		std::ifstream ifile(filepath);
		if (!ifile.is_open())
		{
			LogError("open file '{}' failed.", filepath.string());
			return false;
		}

		try
		{
			nJson json;
			ifile >> json;
			if (!data.fromJson(json))
			{
				LogError("load game data from '{}' failed, json root is not object.", filepath.string());
				return false;
			}

			LogInfo("load game data from '{}' OK.", filepath.string());
		}
		catch (const std::exception& e)
		{
			LogError("load game data from '{}' failed, err = '{}'", filepath.string(), e.what());
			return false;
		}

		return true;
	}
}