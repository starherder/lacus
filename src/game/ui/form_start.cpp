#include "form_start.h"
#include "form_config.h"
#include "game/ui/ui_logic_events.h"

namespace game 
{

FormStart::FormStart(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	load(_context.resPath() / "ui/form_start.xml");

	auto btnStart = getWidget<ui::Button>("btn_start");
	if (btnStart) {
		btnStart->on_click.connect([this](ui::Button* btn) { onStart(btn); });
	}

	auto btnResume = getWidget<ui::Button>("btn_resume");
	if (btnResume) {
		btnResume->on_click.connect([this](ui::Button* btn) { onResume(btn); });
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

void FormStart::onResume(Button* btn)
{
	ui::GuiManager::inst().emitCustomEvent(CustomEventId::Event_SelectScene, 
		{ std::string{"scenes/demos/four.tmj"}, (int)SceneGameMode::GameMode_Test
});

	//on_resume_game.emit();
}

void FormStart::onConfig(Button* btn)
{
	//on_config_game.emit();
	GuiManager::inst().createForm<FormConfig>("from_config", _context);
}

void FormStart::onExit(Button* btn)
{
	on_exit_game.emit();
}

void FormStart::onSizeChanged()
{
}

}