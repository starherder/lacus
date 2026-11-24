#include "system_dialog.h"

#include "ui/form_dialog.h"

namespace game
{
	DeclareEcsSystem(DialogSystem, EcsPriority::Middle);

	DialogSystem::DialogSystem(GameContext& context) : EcsSystem(context)
	{
		_context.dispatcher().sink<EvtStartDialog>().connect<&DialogSystem::onStartDialog>(this);
		_context.dispatcher().sink<EvtDialogNext>().connect<&DialogSystem::onDialogNext>(this);
	}

	DialogSystem::~DialogSystem()
	{
	}

	void DialogSystem::update(float delta)
	{
		auto view = _context.registry().view<CompDialogPlayer>();
		for (auto dialogPlayer : view)
		{
			auto& player = view.get<CompDialogPlayer>(dialogPlayer);
			if (player.dialogue_ticks <= 0)
			{
				continue;
			}

			player.dialogue_ticks -= _context.deltaMs();
			if (player.dialogue_ticks <= 0)
			{
				startNextLine(dialogPlayer, player);
			}
		}
	}

	void DialogSystem::onStartDialog(const EvtStartDialog& e)
	{
		const auto* dialog = DialogManager::inst().getDialog(e.name);
		if (!dialog)
		{
			LogError("DialogSystem::onStartDialog dialog ({}) not found.", e.name);
			return;
		}

		if (dialog->type != "ui")
		{
			LogWarn("DialogSystem::onStartDialog dialog ({}) type ({}) not supported.", e.name, dialog->type);
			return;
		}

		finishAllDialogs();

		auto dialogPlayer = _context.registry().create();
		auto& player = _context.registry().emplace<CompDialogPlayer>(dialogPlayer);
		player.dialog_name = e.name;
		player.dialogue_index = 0;
		player.dialogue_ticks = 0;
		player.game_pause = dialog->game_pause;

		if (player.game_pause)
		{
			_context.pushGamePause();
			_context.dispatcher().trigger(EvtObjectSelection{ entt::null });
		}

		startNextLine(dialogPlayer, player);
	}

	void DialogSystem::onDialogNext(const EvtDialogNext& e)
	{
		auto view = _context.registry().view<CompDialogPlayer>();
		for (auto dialogPlayer : view)
		{
			auto& player = view.get<CompDialogPlayer>(dialogPlayer);
			startNextLine(dialogPlayer, player);
			return;
		}
	}

	void DialogSystem::startNextLine(entt::entity dialogPlayer, CompDialogPlayer& player)
	{
		const auto* dialog = DialogManager::inst().getDialog(player.dialog_name);
		if (!dialog)
		{
			LogWarn("DialogSystem::startNextLine dialog ({}) not found.", player.dialog_name);
			finishDialog(dialogPlayer, player);
			return;
		}

		if (player.dialogue_index >= dialog->dialogue.size())
		{
			finishDialog(dialogPlayer, player);
			return;
		}

		auto form = ui::GuiManager::inst().createForm<FormDialog>("form_dialog", _context);
		if (!form)
		{
			LogError("DialogSystem::startNextLine create form_dialog failed.");
			finishDialog(dialogPlayer, player);
			return;
		}

		const auto lineIndex = player.dialogue_index;
		form->showLine(*dialog, lineIndex);

		player.dialogue_ticks = dialog->dialogue[lineIndex].duration;
		player.dialogue_index++;
	}

	void DialogSystem::finishDialog(entt::entity dialogPlayer, CompDialogPlayer& player)
	{
		if (player.game_pause)
		{
			_context.popGamePause();
			player.game_pause = false;
		}

		ui::GuiManager::inst().closeForm("form_dialog");

		if (_context.registry().valid(dialogPlayer))
		{
			_context.registry().destroy(dialogPlayer);
		}
	}

	void DialogSystem::finishAllDialogs()
	{
		std::vector<entt::entity> players;
		auto view = _context.registry().view<CompDialogPlayer>();
		for (auto dialogPlayer : view)
		{
			players.push_back(dialogPlayer);
		}

		for (auto dialogPlayer : players)
		{
			if (!_context.registry().valid(dialogPlayer))
			{
				continue;
			}

			auto& player = _context.registry().get<CompDialogPlayer>(dialogPlayer);
			finishDialog(dialogPlayer, player);
		}
	}
}
