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

	struct BagItem
	{
		std::string configId;
		int amount = 0;
	};

	using engine::nJson;
	using StringVector = std::vector<std::string>;
	using CardMap = std::map<std::string, HandCard>;
	using CardSet = std::set<std::string>;
	using SceneSet = std::set<std::string>;
	using BagMap = std::map<std::string, BagItem>;

	class GameData
	{
	public:
		// 创建一份空的游戏数据。
		GameData();

		// 禁止移动构造。
		GameData(GameData&&) = delete;

		// 禁止拷贝构造。
		GameData(const GameData&) = delete;

		// 获取当前手牌。
		const CardMap& getHandCards();

		// 替换当前手牌。
		void setHandCards(CardMap handCards);

		// 添加或更新一张手牌。
		void addHandCard(const std::string& cfgid, RoleRank rank = RoleRank::Soldier);

		// 移除一张手牌。
		void removeHandCard(const std::string& cfgid);

		// 清空所有手牌。
		void clearHandCard();

		// 查找一张手牌。
		HandCard* getHandCard(const std::string& cfgid);

		// 设置手牌身份。
		void setHandCardRank(const std::string& cfgid, RoleRank rank);

		// 获取已解锁卡牌。
		const CardSet& getUnlockedCards() const;

		// 替换已解锁卡牌。
		void setUnlockedCards(CardSet cards);

		// 解锁一张卡牌。
		void unlockCard(const std::string& cfgid);

		// 批量解锁卡牌。
		void unlockCards(const StringVector& cfgids);

		// 锁定一张卡牌。
		void lockCard(const std::string& cfgid);

		// 清空已解锁卡牌。
		void clearUnlockedCards();

		// 判断卡牌是否已解锁。
		bool isCardUnlocked(const std::string& cfgid) const;

		// 判断存档是否包含解锁数据。
		bool hasUnlockedCardData() const;

		// 获取当前背包物品。
		const BagMap& getBagItems() const;

		// 添加或叠加背包物品。
		void addBagItem(const std::string& cfgid, int amount = 1);

		// 查找背包物品。
		BagItem* getBagItem(const std::string& cfgid);

		// 查找背包物品。
		const BagItem* getBagItem(const std::string& cfgid) const;

		// 清空背包。
		void clearBagItems();

		// 获取已通关场景。
		const SceneSet& getClearedScenes();

		// 判断场景是否已通关。
		bool isSceneCleared(const std::string& sceneId) const;

		// 标记场景已通关。
		void setSceneCleared(const std::string& sceneId);

		// 序列化游戏数据。
		nJson toJson() const;

		// 反序列化游戏数据。
		void fromJson(const nJson& json);

	private:
		CardMap _handCards;

		CardSet _unlockedCards;

		SceneSet _clearedScenes;

		BagMap _bagItems;

		bool _hasUnlockedCardData = false;
	};
}
