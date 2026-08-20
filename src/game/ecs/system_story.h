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
		bool updateWhenGamePaused() const override { return true; }

	private:
		void onStartStory(const EvtStartStory& e);
		void startNextDialogue(entt::entity storyPlayer, CompStoryPlayer& player);
		void finishStory(entt::entity storyPlayer, CompStoryPlayer& player);
		void followCamera(CompStoryPlayer& player);
		int getDialogueDuration(const StoryDialogue& dialogue) const;
	};
}