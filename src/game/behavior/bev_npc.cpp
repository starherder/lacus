
#include "bev_npc.h"
#include "bevtree/bevtree.h"

#include "game/scene/game_scene.h"
#include "game/play/game_context.h"


namespace game {




	bool BevNode_FindIdlePos::load(const bevtree::XmlNode* node)
	{
		return true;
	}

	Status BevNode_FindIdlePos::update()
	{
		auto context = getBlackboard()->getValue<GameContext*>("context", nullptr);
		if (!context) {
			return Status::Failure;
		}

		auto actor = getBlackboard()->getValue<entt::entity>("entity", entt::null);
		if (actor == entt::null) {
			return Status::Failure;
		}

		//context->registry().get

		//context->pathFinder().findPath();

		return Status::Success;
	}

	void BevNode_FindIdlePos::initialize()
	{

	}

	void BevNode_FindIdlePos::terminate(Status s)
	{

	}

}