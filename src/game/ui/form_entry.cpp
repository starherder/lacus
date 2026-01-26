#include "form_entry.h"
#include "form_config.h"

#include "game/ui/ui_logic_events.h"

namespace game 
{

FormEntry::FormEntry(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	auto group = root()->createChild<Group>("group_bg");
	group->setBgColor(Color::DarkCyan);

	auto btnStart = group->createChild<ui::Button>("btn_start");
	btnStart->setText("start");
	btnStart->setSize({ 200, 50 });
	btnStart->on_click.connect([this](ui::Button* btn) { onStart(btn); });

	auto btnCancel = group->createChild<ui::Button>("btn_cancel");
	btnCancel->setText("cancel");
	btnCancel->setSize({ 200, 50 });
	btnCancel->on_click.connect([this](ui::Button* btn) { close(); });

	setMaximize(false);
	setDragMovable(false);

	auto winsz = ui::GuiManager::inst().windowSize();
	setSize({ winsz.x / 3, winsz.y });
	setPos({ 0, 0 });
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
	auto btnStart = getWidget<Widget>("btn_start");
	if (btnStart)
	{
		auto winsz = size();
		auto dstPos = Vec2{20, winsz.y - 60};
		btnStart->setPos(dstPos);
	}

	auto btnCancel = getWidget<Widget>("btn_cancel");
	if (btnCancel)
	{
		auto winsz = size();
		auto dstPos = Vec2{ btnCancel->size().x + 40, winsz.y - 60 };
		btnCancel->setPos(dstPos);
	}

}

void FormEntry::onWindowResized(const Vec2& size)
{
	auto winsz = ui::GuiManager::inst().windowSize();
	setSize({ winsz.x / 3, winsz.y });
	setPos({ 0, 0 });


}

}