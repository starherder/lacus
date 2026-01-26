#include "form_entry.h"
#include "form_config.h"

#include "game/ui/ui_logic_events.h"

namespace game 
{

FormEntry::FormEntry(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	load(_context.resPath() / "ui/form_entry.xml");

	auto btnEnter = getWidget<ui::Button>("btn_enter");
	if (btnEnter)
	{
		btnEnter->on_click.connect(this, &FormEntry::onStart);
	}

	auto btnLeave = getWidget<ui::Button>("btn_leave");
	if (btnLeave)
	{
		btnLeave->on_click.connect([this](ui::Button* btn) { close(); });
	}
}

FormEntry::~FormEntry()
{
}

void FormEntry::selectScene(int index, const std::string& scene)
{
	_sceneIndex = index;
	_sceneFile = scene;

	//showDescript();
}

void FormEntry::onUpdate(float delta)
{
}

void FormEntry::onStart(Button* btn)
{
	SPDLOG_INFO("FormEntry: onStart, index={}, file={}", _sceneIndex, _sceneFile);

	ui::GuiManager::inst().emitCustomEvent(Event_SelectScene, { _sceneIndex, _sceneFile });
}

void FormEntry::onSizeChanged()
{
}

void FormEntry::onWindowResized(const Vec2& sz)
{
	auto formsz = size();
	auto winsz = ui::GuiManager::inst().windowSize();
	setSize({ formsz.x, winsz.y });
}

}