#pragma once

#include "engine/wrapper.h"

#include <set>

namespace game
{
	using engine::nJson;
	using StringVector = std::vector<std::string>;
	using CardSet = std::multiset<std::string>;
	using SceneSet = std::set<std::string>;

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

		const SceneSet& getClearedScenes() { return _clearedScenes; }
		bool isSceneCleared(const std::string& sceneId) const { return _clearedScenes.contains(sceneId); }
		void setSceneCleared(const std::string& sceneId) { _clearedScenes.insert(sceneId); }

		nJson toJson() const;
		bool fromJson(const nJson& json);

	private:
		CardSet _handCards;

		CardSet _cardGroup;

		SceneSet _clearedScenes;

	};
}