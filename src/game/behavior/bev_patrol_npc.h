#pragma once

#include "bevtree/bevtree.h"
#include "game/ecs/comm_event.h"

namespace game 
{
	using namespace bevtree;

	class BevNode_FindPatrolPos : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;
		
		void initialize() override;

		void terminate(Status s) override;
	private:
		GameContext* _context = nullptr;
		entt::entity _actor;
	};

	class BevNode_PickItem : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;

		void initialize() override;

		void terminate(Status s) override;

	private:
		void onRolePickItemStart(const EvtRolePickItemStart& e);
		void onRoleEnterGrid(const EvtRoleCrossGrid& e);

		Status checkPickUp();

	private:
		GameContext* _context = nullptr;
		entt::entity _actor;

		bool _pickOK = false;
		bool _needCheck = false;
	};

	class BevNode_PatrolMove : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;

		void initialize() override;

		void terminate(Status s) override;

	private:
		void onMotionStop(const EvtMotionStop& e);

	private:
		GameContext* _context = nullptr;
		entt::entity _actor;

		bool _finished = false;
	};

	class BevNode_Idle : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;

		void initialize() override;

		void terminate(Status s) override;

	private:
		GameContext* _context = nullptr;

		float _idleTotalSeconds = 1.0f;
		float _idleCurSeconds = 0.0f;
	};

	class BevNode_RandomEmotion : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;

		void initialize() override;

		void terminate(Status s) override;

	private:
		void resetWaitSeconds();
		void showRandomEmotion();

	private:
		GameContext* _context = nullptr;
		entt::entity _actor = entt::null;

		float _minSeconds = 3.0f;
		float _maxSeconds = 8.0f;
		int64_t _nextTriggerTicks = 0;
	};


	class BevNode_DefendAttack : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;

		void initialize() override;

		void terminate(Status s) override;
	private:
		Status checkEnemy();

	private:
		GameContext* _context = nullptr;
		entt::entity _actor;
	};
}