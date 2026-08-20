#pragma once

#include "game/scene/game_context.h"

#include "utility/i_singleton.h"

namespace game
{
	struct StoryDialogue
	{
		std::string actor;
		std::string emotion;
		std::string word;
		int duration = 0;
	};

	struct StoryConfig
	{
		std::string name;
		std::string desc;
		std::vector<StoryDialogue> dialogue;
	};

	class StoryManager final : public utility::ISingleton<StoryManager>
	{
	public:
		using StoryConfigMap = std::map<std::string, StoryConfig>;

	public:
		StoryManager() = default;
		~StoryManager() = default;

		StoryManager(StoryManager&&) = delete;
		StoryManager(const StoryManager&) = delete;

		void init(GameContext* context);
		bool load(const fs::path& storydir);
		bool loadStoryFile(const fs::path& storyfile);

		const StoryConfig* getStory(const std::string& name) const;
		const StoryConfigMap& getStories() const { return _stories; }

	private:
		int parseDuration(const nJson& item) const;

	private:
		GameContext* _context = nullptr;
		StoryConfigMap _stories;
		fs::path _storyPath;
	};
}
