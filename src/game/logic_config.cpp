#include "logic_config.h"
#include "engine/wrapper.h"
#include "nlohmann/json.hpp"

namespace game
{

	bool LogicConfig::loadEmotion(const fs::path& filepath)
	{
		bool res = _jsonConfig.load(filepath);
		if (!res)
		{
			LogError("LogicConfig::loadEmotion {} failed.", filepath.string());
			return false;
		}

		auto& json = _jsonConfig.getJson();
		if (!json.is_array())
		{
			LogError("LogicConfig::loadEmotion: json is not an array.");
			return false;
		}

		_emotions.clear();
		for (const auto& item : json)
		{
			auto name = item.value("name", "");
			auto texture = item.value("texture", "");
			if (!name.empty())
			{
				_emotions[name] = texture;
			}
		}

		return true;
	}

	const std::string* LogicConfig::getEmotion(const std::string& name) const
	{
		auto it = _emotions.find(name);
		if (it != _emotions.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	const LogicConfig::EmotionTextureMap& LogicConfig::getAllEmotion() const
	{
		return _emotions;
	}

	std::vector<std::string> LogicConfig::getAllEmotionNames() const
	{
		std::vector<std::string> names;
		names.reserve(_emotions.size());
		for (const auto& [name, _] : _emotions)
		{
			names.push_back(name);
		}
		return names;
	}

}
