#include "system_story.h"

#include "game/game_config.h"
#include "game/scene/story_manager.h"

namespace game 
{
	DeclareEcsSystem(StorySystem, EcsPriority::Middle);

	StorySystem::StorySystem(GameContext& context) : EcsSystem(context)
	{
		_context.dispatcher().sink<EvtStartStory>().connect<&StorySystem::onStartStory>(this);
	}

	StorySystem::~StorySystem()
	{
		_context.dispatcher().sink<EvtStartStory>().disconnect<&StorySystem::onStartStory>(this);
	}

	void StorySystem::update(float delta)
	{
		auto view = _context.registry().view<CompStoryPlayer>();
		for (auto storyPlayer : view)
		{
			auto& player = view.get<CompStoryPlayer>(storyPlayer);
			followCamera(player);

			player.dialogue_ticks -= _context.deltaTicks();
			if (player.dialogue_ticks <= 0)
			{
				startNextDialogue(storyPlayer, player);
			}
		}
	}

	void StorySystem::onStartStory(const EvtStartStory& e)
	{
		const auto* story = StoryManager::inst().getStory(e.name);
		if (!story)
		{
			LogError("StorySystem::onStartStory story ({}) not found.", e.name);
			return;
		}

		for (const auto& dialogue : story->dialogue)
		{
			auto actorIt = e.actors.find(dialogue.actor);
			if (actorIt == e.actors.end() || !_context.registry().valid(actorIt->second))
			{
				LogError("StorySystem::onStartStory actor ({}) invalid in story ({}).", dialogue.actor, e.name);
				return;
			}
		}

		auto storyPlayer = _context.registry().create();
		auto& player = _context.registry().emplace<CompStoryPlayer>(storyPlayer);
		player.story_name = e.name;
		player.actors = e.actors;
		player.dialogue_index = 0;
		player.dialogue_ticks = 0;
		player.camera_follow = story->camera_follow;
		player.game_pause = story->game_pause;

		if (player.game_pause)
		{
			_context.pushGamePause();
			_context.dispatcher().trigger(EvtObjectSelection{ entt::null });
		}

		startNextDialogue(storyPlayer, player);
	}

	void StorySystem::startNextDialogue(entt::entity storyPlayer, CompStoryPlayer& player)
	{
		const auto* story = StoryManager::inst().getStory(player.story_name);
		if (!story)
		{
			LogWarn("StorySystem::startNextDialogue story ({}) not found.", player.story_name);
			finishStory(storyPlayer, player);
			return;
		}

		if (player.dialogue_index >= story->dialogue.size())
		{
			finishStory(storyPlayer, player);
			return;
		}

		const auto& dialogue = story->dialogue[player.dialogue_index];
		auto actorIt = player.actors.find(dialogue.actor);
		if (actorIt == player.actors.end() || !_context.registry().valid(actorIt->second))
		{
			LogWarn("StorySystem::startNextDialogue actor ({}) invalid.", dialogue.actor);
			finishStory(storyPlayer, player);
			return;
		}

		player.camera_target = actorIt->second;
		followCamera(player);

		EvtShowBubble evt;
		evt.actor = actorIt->second;
		evt.text = dialogue.word;
		evt.emotion = dialogue.emotion;
		_context.dispatcher().trigger(evt);

		player.dialogue_ticks = getDialogueDuration(dialogue);
		player.dialogue_index++;
	}

	void StorySystem::finishStory(entt::entity storyPlayer, CompStoryPlayer& player)
	{
		if (player.game_pause)
		{
			_context.popGamePause();
			player.game_pause = false;
		}

		if (_context.registry().valid(storyPlayer))
		{
			_context.registry().destroy(storyPlayer);
		}
	}

	void StorySystem::followCamera(CompStoryPlayer& player)
	{
		if (!player.camera_follow || !_context.registry().valid(player.camera_target))
		{
			return;
		}

		auto transform = _context.registry().try_get<CompTransform>(player.camera_target);
		if (!transform)
		{
			return;
		}

		player.camera_target_pos = transform->position - _context.camera().getSize() / 2.0f;

		const auto& curPos = _context.camera().getPos();
		auto offset = player.camera_target_pos - curPos;
		float distance = glm::length(offset);
		if (distance <= 1.0f)
		{
			_context.camera().setPos(player.camera_target_pos);
			return;
		}

		constexpr float cameraSpeed = 1200.0f;
		float step = cameraSpeed * std::max(0.0f, _context.frameTicker().deltaSeconds());
		if (step >= distance)
		{
			_context.camera().setPos(player.camera_target_pos);
			return;
		}

		_context.camera().setPos(curPos + offset / distance * step);
	}

	int StorySystem::getDialogueDuration(const StoryDialogue& dialogue) const
	{
		if (dialogue.duration > 0)
		{
			return dialogue.duration;
		}
		return _context.gameConfig().bubble.lifetime_ms;
	}
}