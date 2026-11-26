#include "game_data.h"

#include <nlohmann/json.hpp>

namespace game
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HandCard, configId, rank)

	nJson GameData::toJson() const
	{
		return {
			{ "hand_cards", _handCards },
			{ "cleared_scenes", _clearedScenes },
		};
	}

	void GameData::fromJson(const nJson& json)
	{
		auto handCards = json.value("hand_cards", CardMap{});
		auto clearedScenes = json.value("cleared_scenes", SceneSet{});

		_handCards = std::move(handCards);
		_clearedScenes = std::move(clearedScenes);
	}
}
