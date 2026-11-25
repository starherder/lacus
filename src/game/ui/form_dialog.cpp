#include "form_dialog.h"

#include "game/ecs/comm_event.h"
#include "game/scene/object_manager.h"
#include "utility/translator.h"

#include <format>

namespace game
{
	FormDialog::FormDialog(const std::string& name, GameContext& context)
		: FormLogicBase(name, context)
	{
		load(_context.resPath() / "ui/form_dialog.xml");
		initUI();
	}

	FormDialog::~FormDialog()
	{
	}

	void FormDialog::initUI()
	{
		_lblAvatarLeft = getWidget<Label>("lbl_avatar_left");
		_lblAvatarRight = getWidget<Label>("lbl_avatar_right");
		_lblActorLeft = getWidget<Label>("lbl_actor_left");
		_lblActorRight = getWidget<Label>("lbl_actor_right");
		_lblSpeaker = getWidget<Label>("lbl_speaker");
		_lblProgress = getWidget<Label>("lbl_progress");
		_txtDialog = getWidget<TextBox>("txt_dialog");
		_btnNext = getWidget<Button>("btn_next");

		if (_btnNext)
		{
			_btnNext->on_click.connect(this, &FormDialog::onNext);
		}
	}

	void FormDialog::showLine(const DialogConfig& dialog, size_t index)
	{
		if (_dialogName != dialog.name)
		{
			_dialogName = dialog.name;
			_actorSides.clear();
			_nextSide = 0;
		}

		if (index >= dialog.dialogue.size())
		{
			return;
		}

		const auto& line = dialog.dialogue[index];
		auto side = getActorSide(line.actor);

		clearActor(_lblAvatarLeft, _lblActorLeft);
		clearActor(_lblAvatarRight, _lblActorRight);

		if (side == 0)
		{
			showActor(_lblAvatarLeft, _lblActorLeft, line);
		}
		else
		{
			showActor(_lblAvatarRight, _lblActorRight, line);
		}

		if (_lblSpeaker)
		{
			_lblSpeaker->setText(Trans(line.actor));
		}
		if (_txtDialog)
		{
			_txtDialog->setText(Trans(line.word));
		}
		if (_lblProgress)
		{
			_lblProgress->setText(std::format("{}/{}", index + 1, dialog.dialogue.size()));
		}
	}

	void FormDialog::onNext(Button* btn)
	{
		_context.dispatcher().trigger(EvtDialogNext{});
	}

	void FormDialog::clearActor(Label* avatar, Label* name)
	{
		if (avatar)
		{
			avatar->setVisible(false);
			avatar->setTexTile(nullptr);
		}
		if (name)
		{
			name->setVisible(false);
			name->setText("");
		}
	}

	void FormDialog::showActor(Label* avatar, Label* name, const DialogLine& line)
	{
		std::string avatarCfg = line.avatar;
		if (avatarCfg.empty() && ObjectManager::inst().findObjectCfg(line.actor))
		{
			const auto& props = ObjectManager::inst().getObjectCfgProperties(line.actor);
			auto icon = props.get<std::string>("icon");
			if (icon)
			{
				avatarCfg = *icon;
			}
		}

		if (avatar)
		{
			avatar->setVisible(true);
			avatar->setTexTile(avatarCfg);
		}
		if (name)
		{
			name->setVisible(true);
			name->setText(Trans(line.actor));
		}
	}

	int FormDialog::getActorSide(const std::string& actor)
	{
		auto it = _actorSides.find(actor);
		if (it != _actorSides.end())
		{
			return it->second;
		}

		auto side = _nextSide;
		_actorSides[actor] = side;
		_nextSide = 1 - _nextSide;
		return side;
	}
}
