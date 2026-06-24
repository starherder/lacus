#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "engine/config.h"

namespace game
{
	using namespace engine;

	class LogicConfig final
	{
		using EmotionTextureMap = std::unordered_map<std::string, std::string>;

	public:
		LogicConfig() = default;
		~LogicConfig() = default;

		bool loadEmotion(const fs::path& filepath);

		const std::string* getEmotion(const std::string& name) const;

		const EmotionTextureMap& getAllEmotion() const;

		std::vector<std::string> getAllEmotionNames() const;

	private:
		JsonConfig _jsonConfig;
		EmotionTextureMap _emotions;
	};

}
