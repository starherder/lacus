#pragma once

#include "engine/wrapper.h"
#include "game/ecs/comm_comp.h"

#include <map>
#include <set>

namespace game
{
	struct HandCard 
	{
		std::string configId;
		RoleRank rank = RoleRank::Soldier;
	};

	using engine::nJson;
	using StringVector = std::vector<std::string>;
	using CardMap = std::map<std::string, HandCard>;
	using SceneSet = std::set<std::string>;

	class GameData
	{
	public:
		GameData() = default;
		GameData(GameData&&) = delete;
		GameData(const GameData&) = delete;

		const CardMap& getHandCards() { return _handCards; }
		void setHandCards(CardMap handCards) { _handCards = std::move(handCards); }
		void addHandCard(const std::string& cfgid, RoleRank rank = RoleRank::Soldier) { _handCards.insert_or_assign(cfgid, HandCard{cfgid, rank}); }
		void removeHandCard(const std::string& cfgid) { _handCards.erase(cfgid); }
		void clearHandCard() { _handCards.clear(); }
		HandCard* getHandCard(const std::string& cfgid) {
			auto it = _handCards.find(cfgid);
			return (it == _handCards.end()) ? nullptr : &it->second;
		}
		void setHandCardRank(const std::string& cfgid, RoleRank rank) { addHandCard(cfgid, rank); }

		const SceneSet& getClearedScenes() { return _clearedScenes; }
		bool isSceneCleared(const std::string& sceneId) const { return _clearedScenes.contains(sceneId); }
		void setSceneCleared(const std::string& sceneId) { _clearedScenes.insert(sceneId); }

		nJson toJson() const;
		void fromJson(const nJson& json);

	private:
		CardMap _handCards;

		SceneSet _clearedScenes;

	};
}
