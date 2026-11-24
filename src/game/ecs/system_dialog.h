#pragma once

#include "system_utils.h"
#include "game/scene/dialog_manager.h"

namespace game
{
	class DialogSystem : public EcsSystem
	{
	public:
		DialogSystem(GameContext& context);
		~DialogSystem();

		void update(float delta) override;
		bool updateWhenGamePaused() const override { return true; }

	private:
		void onStartDialog(const EvtStartDialog& e);
		void onDialogNext(const EvtDialogNext& e);
		void startNextLine(entt::entity dialogPlayer, CompDialogPlayer& player);
		void finishDialog(entt::entity dialogPlayer, CompDialogPlayer& player);
		void finishAllDialogs();
	};
}
