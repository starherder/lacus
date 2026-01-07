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
		void onRolePickItemStart(const RolePickItemStart& e);
		void onRoleEnterGrid(const RoleCrossGrid& e);

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
		void onMotionStop(const MotionStop& e);

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
}