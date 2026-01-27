#pragma once

#include "game/scene/game_context.h"
#include "game/scene/object_factory.h"
#include "game/ecs/comm_event.h"

#include <set>

namespace game
{
	using StringVector = std::vector<std::string>;
	using CardSet = std::multiset<std::string>;

	class GameData
	{
	public:
		GameData() = default;
		GameData(GameData&&) = delete;
		GameData(const GameData&) = delete;

		const CardSet& getHandCards() { return _handCards; }
		
		void addHandCard(const std::string& cfgid) { _handCards.insert(cfgid); }

		void removeHandCard(const std::string& cfgid) { _handCards.erase(cfgid); }

	private:
		CardSet _handCards;

	};
}