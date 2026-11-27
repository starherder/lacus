#include "game_data.h"

#include <nlohmann/json.hpp>
#include <utility>

namespace game
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HandCard, configId, rank)

	GameData::GameData() = default;

	const CardMap& GameData::getHandCards()
	{
		return _handCards;
	}

	void GameData::setHandCards(CardMap handCards)
	{
		_handCards = std::move(handCards);
	}

	void GameData::addHandCard(const std::string& cfgid, RoleRank rank)
	{
		_handCards.insert_or_assign(cfgid, HandCard{ cfgid, rank });
	}

	void GameData::removeHandCard(const std::string& cfgid)
	{
		_handCards.erase(cfgid);
	}

	void GameData::clearHandCard()
	{
		_handCards.clear();
	}

	HandCard* GameData::getHandCard(const std::string& cfgid) 
	{
		auto it = _handCards.find(cfgid);
		return (it == _handCards.end()) ? nullptr : &it->second;
	}

	void GameData::setHandCardRank(const std::string& cfgid, RoleRank rank)
	{
		addHandCard(cfgid, rank);
	}

	const CardSet& GameData::getUnlockedCards() const
	{
		return _unlockedCards;
	}

	void GameData::setUnlockedCards(CardSet cards)
	{
		_unlockedCards = std::move(cards);
		_hasUnlockedCardData = true;
	}

	void GameData::unlockCard(const std::string& cfgid)
	{
		_unlockedCards.insert(cfgid);
		_hasUnlockedCardData = true;
	}

	void GameData::unlockCards(const StringVector& cfgids)
	{
		_unlockedCards.insert(cfgids.begin(), cfgids.end());
		_hasUnlockedCardData = true;
	}

	void GameData::lockCard(const std::string& cfgid)
	{
		_unlockedCards.erase(cfgid);
		_hasUnlockedCardData = true;
	}

	void GameData::clearUnlockedCards()
	{
		_unlockedCards.clear();
		_hasUnlockedCardData = true;
	}

	bool GameData::isCardUnlocked(const std::string& cfgid) const
	{
		return _unlockedCards.contains(cfgid);
	}

	bool GameData::hasUnlockedCardData() const
	{
		return _hasUnlockedCardData;
	}

	const SceneSet& GameData::getClearedScenes()
	{
		return _clearedScenes;
	}

	bool GameData::isSceneCleared(const std::string& sceneId) const
	{
		return _clearedScenes.contains(sceneId);
	}

	void GameData::setSceneCleared(const std::string& sceneId)
	{
		_clearedScenes.insert(sceneId);
	}
	
	nJson GameData::toJson() const
	{
		return {
			{ "hand_cards", _handCards },
			{ "unlocked_cards", _unlockedCards },
			{ "cleared_scenes", _clearedScenes },
		};
	}

	void GameData::fromJson(const nJson& json)
	{
		auto handCards = json.value("hand_cards", CardMap{});
		auto unlockedCards = json.value("unlocked_cards", CardSet{});
		auto clearedScenes = json.value("cleared_scenes", SceneSet{});

		_handCards = std::move(handCards);
		_unlockedCards = std::move(unlockedCards);
		_clearedScenes = std::move(clearedScenes);
		_hasUnlockedCardData = json.contains("unlocked_cards");

		if (!_hasUnlockedCardData)
		{
			for (const auto& [cfgid, handCard] : _handCards)
			{
				_unlockedCards.insert(cfgid);
			}
		}
	}
}
