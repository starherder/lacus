#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "engine/config.h"

namespace game
{
	using namespace engine;

	enum class EmotionResourceType
	{
		Texture,
		Animation,
	};

	struct EmotionConfig
	{
		EmotionResourceType type = EmotionResourceType::Texture;
		std::string resource;
	};

	class LogicConfig final
	{
	public:
		using EmotionConfigMap = std::unordered_map<std::string, EmotionConfig>;

	public:
		LogicConfig() = default;
		~LogicConfig() = default;

		bool loadEmotion(const fs::path& filepath);

		const EmotionConfig* getEmotion(const std::string& name) const;

		const EmotionConfigMap& getAllEmotion() const;

		std::vector<std::string> getAllEmotionNames() const;

	private:
		JsonConfig _jsonConfig;
		EmotionConfigMap _emotions;
	};

}
