#pragma once

#include "game_play.h"


namespace game
{
	class GamePlayAutoChess : public GamePlay, public signals::SlotHandler
	{
	public:
		GamePlayAutoChess(GameContext& context);
		~GamePlayAutoChess() = default;

		GamePlayType getType() override { return GamePlayType::GamePlay_AutoChess; };

		void update(float deltaTime) override;
		void draw() override;

		entt::entity getSelectEntity() override { return _selectEntity; }
		void setSelectEntity(entt::entity ent) { _selectEntity = ent; }

		bool objectDragable() { return _canDragObject; }
		void setObjectDragable(bool canDrag) { _canDragObject = canDrag; }

	private:
		void onMouseLeftPressed(const Vec2& pos);
		void onMouseLeftRelease(const Vec2& pos);
		void onMouseLeftDrag(const Vec2& pos, const Vec2& offset);
		void onMouseMotion(const Vec2& pos, const Vec2& offset);

		void onKeyDown(KeyCode key);

		void onMouseLeftClick(const Vec2& pos);
		void onMouseRightClick(const Vec2& pos);

		void onMouseLeftDragStart(const Vec2& pos);
		void onMouseLeftDragFinish(const Vec2& pos);

		void moveSelectActor(const Vec2& pos);
		bool dragSelectActor(const Vec2& pos);
		bool dropSelectActor(const Vec2& pos);
		bool dragSelectActorInProgress(const Vec2& pos);

		bool canDropToPos(const Vec2& pos);

		void onRoleSelect();
		void onRoleUnselect();

	private:
		entt::entity _selectEntity = entt::null;

		bool _canDragObject = true;
	};

}