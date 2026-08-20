#include "story_manager.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace game
{
	void StoryManager::init(GameContext* context)
	{
		_context = context;
		_stories.clear();
		_storyPath.clear();
	}

	bool StoryManager::load(const fs::path& storydir)
	{
		assert(_context);

		_storyPath = storydir;
		_stories.clear();

		if (!fs::exists(storydir))
		{
			LogError("StoryManager::load story dir ({}) not exists.", storydir.string());
			return false;
		}

		bool result = true;
		for (const auto& entry : fs::directory_iterator(storydir))
		{
			if (!entry.is_regular_file() || entry.path().extension() != ".json")
			{
				continue;
			}

			result = loadStoryFile(entry.path()) && result;
		}

		return result;
	}

	bool StoryManager::loadStoryFile(const fs::path& storyfile)
	{
		assert(_context);

		nJson json;
		std::ifstream ifile(storyfile.string());
		if (!ifile.is_open())
		{
			LogError("open file '{}' failed.", storyfile.string());
			return false;
		}

		try
		{
			ifile >> json;
			LogInfo("load json from '{}' OK.", storyfile.string());
		}
		catch (const std::exception& e)
		{
			LogError("load json form '{}' failed, err = '{}'", storyfile.string(), e.what());
			return false;
		}

		if (!json.is_array())
		{
			LogError("StoryManager::loadStoryFile {} failed, not array.", storyfile.string());
			return false;
		}

		for (const auto& item : json)
		{
			StoryConfig config;
			config.name = item.value("name", "");
			config.desc = item.value("desc", "");

			if (config.name.empty())
			{
				LogWarn("StoryManager::loadStoryFile skip story without name.");
				continue;
			}

			if (!item.contains("dialogue") || !item["dialogue"].is_array())
			{
				LogWarn("StoryManager::loadStoryFile skip story ({}) without dialogue.", config.name);
				continue;
			}

			for (const auto& dialogueItem : item["dialogue"])
			{
				StoryDialogue dialogue;
				dialogue.actor = dialogueItem.value("actor", "");
				dialogue.emotion = dialogueItem.value("emotion", "");
				dialogue.word = dialogueItem.value("word", "");
				dialogue.duration = parseDuration(dialogueItem);

				if (dialogue.actor.empty())
				{
					LogWarn("StoryManager::loadStoryFile skip dialogue without actor in story ({}).", config.name);
					continue;
				}

				config.dialogue.push_back(std::move(dialogue));
			}

			_stories[config.name] = std::move(config);
		}

		return true;
	}

	const StoryConfig* StoryManager::getStory(const std::string& name) const
	{
		auto it = _stories.find(name);
		if (it != _stories.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	int StoryManager::parseDuration(const nJson& item) const
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
				LogWarn("StoryManager::parseDuration failed, err = '{}'", e.what());
			}
		}

		return 0;
	}
}
