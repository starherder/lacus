#include "game_data.h"

#include <nlohmann/json.hpp>

namespace game
{
	template<class T>
	static T jsonToStringSet(const nJson& json, const std::string& name)
	{
		T result;

		if (!json.contains(name) || !json[name].is_array())
		{
			return result;
		}

		for (const auto& item : json[name])
		{
			if (item.is_string())
			{
				result.insert(item.get<std::string>());
			}
		}

		return result;
	}

	nJson GameData::toJson() const
	{
		nJson json;
		json["hand_cards"] = StringVector(_handCards.begin(), _handCards.end());
		json["card_group"] = StringVector(_cardGroup.begin(), _cardGroup.end());
		json["cleared_scenes"] = StringVector(_clearedScenes.begin(), _clearedScenes.end());

		return json;
	}

	bool GameData::fromJson(const nJson& json)
	{
		if (!json.is_object())
		{
			return false;
		}

		_handCards = jsonToStringSet<CardSet>(json, "hand_cards");
		_cardGroup = jsonToStringSet<CardSet>(json, "card_group");
		_clearedScenes = jsonToStringSet<SceneSet>(json, "cleared_scenes");

		return true;
	}
}