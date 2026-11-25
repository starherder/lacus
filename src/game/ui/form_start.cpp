#include "form_start.h"
#include "form_config.h"

#include "game/ui/ui_logic_events.h"
#include "game/logic/game_logic.h"
#include "game/scene/game_context.h"

namespace game 
{

FormStart::FormStart(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	load(_context.resPath() / "ui/form_start.xml");

	auto btnStart = getWidget<ui::Button>("btn_start");
	if (btnStart) {
		btnStart->on_click.connect([this](ui::Button* btn) { onStart(btn); });
	}

	auto btnBot = getWidget<ui::Button>("btn_tile_battle");
	if (btnBot) {
		btnBot->on_click.connect([this](ui::Button* btn) { onTileBattle(btn); });
	}
	
	auto btnAutoChess = getWidget<ui::Button>("btn_auto_chess");
	if (btnAutoChess) {
		btnAutoChess->on_click.connect([this](ui::Button* btn) { onAutoChess(btn); });
	}

	auto btnConfig = getWidget<ui::Button>("btn_config");
	if (btnConfig) {
		btnConfig->on_click.connect([this](ui::Button* btn) { onConfig(btn); });
	}

	auto btnExit = getWidget<ui::Button>("btn_exit");
	if (btnExit) {
		btnExit->on_click.connect([this](ui::Button* btn) { onExit(btn); });
	}
}

FormStart::~FormStart()
{
}

void FormStart::onUpdate(float delta)
{
}

void FormStart::onStart(Button* btn)
{
	on_start_game.emit();
}

void FormStart::onTileBattle(Button* btn)
{
	_context.gameLogic().switchScene("demo_4_1");
	_context.gameLogic().setGameMode(SceneGameMode::GameMode_Test);
}

void FormStart::onAutoChess(Button* btn)
{
	_context.gameLogic().switchScene("demo_4");
	_context.gameLogic().setGameMode(SceneGameMode::GameMode_Test);
}

void FormStart::onConfig(Button* btn)
{
	GuiManager::inst().showForm<FormConfig>("from_config", _context);
}

void FormStart::onExit(Button* btn)
{
	on_exit_game.emit();
}

void FormStart::onSizeChanged()
{
}

}