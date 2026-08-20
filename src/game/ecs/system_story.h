#pragma once

#include "system_utils.h"
#include "game/scene/story_manager.h"

namespace game 
{

	class StorySystem : public EcsSystem
	{
	public:
		StorySystem(GameContext& context);
		~StorySystem();

		void update(float delta) override;

	private:
		void onStartStory(const EvtStartStory& e);
		void startNextDialogue(entt::entity storyPlayer, CompStoryPlayer& player);
		int getDialogueDuration(const StoryDialogue& dialogue) const;
	};
}