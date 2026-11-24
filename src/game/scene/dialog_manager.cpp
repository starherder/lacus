#include "dialog_manager.h"

#include <algorithm>
#include <cctype>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>

namespace game
{
	void DialogManager::init(GameContext* context)
	{
		_context = context;
		_dialogs.clear();
		_dialogPath.clear();
	}

	bool DialogManager::load(const fs::path& dialogdir)
	{
		assert(_context);

		_dialogPath = dialogdir;
		_dialogs.clear();

		if (!fs::exists(dialogdir))
		{
			LogError("DialogManager::load dialog dir ({}) not exists.", dialogdir.string());
			return false;
		}

		bool result = true;
		for (const auto& entry : fs::directory_iterator(dialogdir))
		{
			if (!entry.is_regular_file() || entry.path().extension() != ".json")
			{
				continue;
			}

			result = loadDialogFile(entry.path()) && result;
		}

		return result;
	}

	bool DialogManager::loadDialogFile(const fs::path& dialogfile)
	{
		assert(_context);

		nJson json;
		std::ifstream ifile(dialogfile.string());
		if (!ifile.is_open())
		{
			LogError("open file '{}' failed.", dialogfile.string());
			return false;
		}

		try
		{
			ifile >> json;
			LogInfo("load json from '{}' OK.", dialogfile.string());
		}
		catch (const std::exception& e)
		{
			LogError("load json form '{}' failed, err = '{}'", dialogfile.string(), e.what());
			return false;
		}

		if (!json.is_array())
		{
			LogError("DialogManager::loadDialogFile {} failed, not array.", dialogfile.string());
			return false;
		}

		for (const auto& item : json)
		{
			DialogConfig config;
			config.name = item.value("name", "");
			config.type = item.value("type", "ui");
			config.desc = item.value("desc", "");
			config.camera_follow = parseBool(item, "camera_follow");
			config.game_pause = parseBool(item, "game_pause");

			if (config.name.empty())
			{
				LogWarn("DialogManager::loadDialogFile skip dialog without name.");
				continue;
			}

			if (!item.contains("dialogue") || !item["dialogue"].is_array())
			{
				LogWarn("DialogManager::loadDialogFile skip dialog ({}) without dialogue.", config.name);
				continue;
			}

			for (const auto& dialogueItem : item["dialogue"])
			{
				DialogLine dialogue;
				dialogue.actor = dialogueItem.value("actor", "");
				dialogue.emotion = dialogueItem.value("emotion", "");
				dialogue.word = dialogueItem.value("word", "");
				dialogue.avatar = parseAvatar(dialogueItem, dialogue.actor);
				dialogue.duration = parseDuration(dialogueItem);

				if (dialogue.actor.empty())
				{
					LogWarn("DialogManager::loadDialogFile skip dialogue without actor in dialog ({}).", config.name);
					continue;
				}

				config.dialogue.push_back(std::move(dialogue));
			}

			_dialogs[config.name] = std::move(config);
		}

		return true;
	}

	const DialogConfig* DialogManager::getDialog(const std::string& name) const
	{
		auto it = _dialogs.find(name);
		if (it != _dialogs.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	int DialogManager::parseDuration(const nJson& item) const
	{
		if (!item.contains("duration"))
		{
			return 0;
		}

		const auto& value = item["duration"];
		if (value.is_number_integer())
		{
			return value.get<int>();
		}
		if (value.is_string())
		{
			try
			{
				return std::stoi(value.get<std::string>());
			}
			catch (const std::exception& e)
			{
				LogWarn("DialogManager::parseDuration failed, err = '{}'", e.what());
			}
		}

		return 0;
	}

	bool DialogManager::parseBool(const nJson& item, const std::string& key, bool defaultValue) const
	{
		if (!item.contains(key))
		{
			return defaultValue;
		}

		const auto& value = item[key];
		if (value.is_boolean())
		{
			return value.get<bool>();
		}
		if (value.is_number_integer())
		{
			return value.get<int>() != 0;
		}
		if (value.is_string())
		{
			auto str = value.get<std::string>();
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) {
				return static_cast<char>(std::tolower(ch));
			});

			if (str == "true" || str == "1" || str == "yes" || str == "on")
			{
				return true;
			}
			if (str == "false" || str == "0" || str == "no" || str == "off")
			{
				return false;
			}
		}

		LogWarn("DialogManager::parseBool failed, key = '{}'", key);
		return defaultValue;
	}

	std::string DialogManager::parseAvatar(const nJson& item, const std::string& actor) const
	{
		auto avatar = item.value("avatar", "");
		if (!avatar.empty())
		{
			return avatar;
		}

		auto pos = actor.find_last_of('_');
		if (pos == std::string::npos || pos + 1 >= actor.size())
		{
			return "";
		}

		auto index = actor.substr(pos + 1);
		if (std::all_of(index.begin(), index.end(), [](unsigned char ch) { return std::isdigit(ch); }))
		{
			return std::format("role:{}-0", index);
		}

		return "";
	}
}
