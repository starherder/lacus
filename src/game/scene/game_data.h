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
		void clearHandCard() { _handCards.clear(); }

		const CardSet& getCardGroup() { return _cardGroup; }
		void addToCardGroup(const std::string& cfgid) { _cardGroup.insert(cfgid); }
		void removeFromCardGroup(const std::string& cfgid) { _cardGroup.erase(cfgid); }
		void clearCardCard() { _cardGroup.clear(); }

	private:
		CardSet _handCards;

		CardSet _cardGroup;

	};
}