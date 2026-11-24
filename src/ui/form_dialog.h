#pragma once

#include "game/ui/form_logic_base.h"
#include "game/scene/dialog_manager.h"

namespace game
{
	class FormDialog : public FormLogicBase
	{
	public:
		FormDialog(const std::string& name) = delete;
		FormDialog(const std::string& name, GameContext& context);
		~FormDialog();

		void showLine(const DialogConfig& dialog, size_t index);

	private:
		void initUI();
		void onNext(Button* btn);
		void clearActor(Label* avatar, Label* name);
		void showActor(Label* avatar, Label* name, const DialogLine& line);
		int getActorSide(const std::string& actor);

	private:
		Label* _lblAvatarLeft = nullptr;
		Label* _lblAvatarRight = nullptr;
		Label* _lblActorLeft = nullptr;
		Label* _lblActorRight = nullptr;
		Label* _lblSpeaker = nullptr;
		Label* _lblProgress = nullptr;
		TextBox* _txtDialog = nullptr;
		Button* _btnNext = nullptr;

		std::string _dialogName;
		std::map<std::string, int> _actorSides;
		int _nextSide = 0;
	};
}
