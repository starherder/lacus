#pragma once

#include "game/scene/game_context.h"
#include "game/scene/object_factory.h"
#include "game/ecs/comm_event.h"

namespace game
{
	using StringVector = std::vector<std::string>;


	class GameData
	{
	public:
		GameData() = default;
		GameData(GameData&&) = delete;
		GameData(const GameData&) = delete;

		const StringVector& getHandCards() { return _handCards; }
		void addHandCards(const std::string& cfgid) { _handCards.push_back(cfgid); }

	private:
		StringVector _handCards;

	};
}