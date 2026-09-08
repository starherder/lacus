#pragma once

#include "engine/wrapper.h"
#include "utility/i_singleton.h"

#include <filesystem>

namespace game
{
	class GameData;

	class SaveManager final : public utility::ISingleton<SaveManager>
	{
	public:
		SaveManager() = default;
		~SaveManager() = default;

		SaveManager(SaveManager&&) = delete;
		SaveManager(const SaveManager&) = delete;

		bool saveGameData(const std::filesystem::path& filepath, const GameData& data);
		bool loadGameData(const std::filesystem::path& filepath, GameData& data);
	};
}