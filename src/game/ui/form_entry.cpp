#include "form_entry.h"
#include "form_config.h"

namespace game 
{

FormEntry::FormEntry(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	root()->setBgColor(Color::Light);

	_lblTitle = root()->createChild<Label>("title");
	_lblTitle->setFont("fonts/Vonwaon.ttf", 40);
	_lblTitle->setBgColor(Color::Light);
	_lblTitle->setBorderColor(Color::Light);
	_lblTitle->setTextColor(Color::DarkGreen);
	_lblTitle->setText("Lacus");
	_lblTitle->setSize({ 300, 100 });
	
	_btnStart = root()->createChild<ui::Button>("btn_start");
	_btnStart->setText("start");
	_btnStart->setSize({ 200, 50 });
	_btnStart->on_click.connect([this](ui::Button* btn) { onStart(btn); });

	_btnResume = root()->createChild<ui::Button>("btn_resume");
	_btnResume->setText("resume");
	_btnResume->setSize({ 200, 50 });
	_btnResume->on_click.connect([this](ui::Button* btn) { onResume(btn); });

	_btnConfig = root()->createChild<ui::Button>("btn_config");
	_btnConfig->setText("config");
	_btnConfig->setSize({ 200, 50 });
	_btnConfig->on_click.connect([this](ui::Button* btn) { onConfig(btn); });

	_btnExit = root()->createChild<ui::Button>("btn_exit");
	_btnExit->setText("exit");
	_btnExit->setSize({ 200, 50 });
	_btnExit->on_click.connect([this](ui::Button* btn) { onExit(btn); });

	setMaximize(true);

}

FormEntry::~FormEntry()
{
}

void FormEntry::onUpdate(float delta)
{
}

void FormEntry::onStart(Button* btn)
{
	on_start_game.emit();
}

void FormEntry::onResume(Button* btn)
{
	on_resume_game.emit();
}

void FormEntry::onConfig(Button* btn)
{
	//on_config_game.emit();
	GuiManager::inst().showForm<FormConfig>("from_config", _context);
}

void FormEntry::onExit(Button* btn)
{
	on_exit_game.emit();
}

void FormEntry::onSizeChanged()
{
	spdlog::info("FormEntry::onWindowResized");

	auto x = size().x / 2 - _lblTitle->size().x / 2;
	auto y = size().y / 2 - 160;
	_lblTitle->setPos({ x, y });

	x = size().x / 2 - _btnStart->size().x / 2;
	y = _lblTitle->pos().y + _lblTitle->size().y + 50;
	_btnStart->setPos({ x, y });

	x = size().x / 2 - _btnResume->size().x / 2;
	y = _btnStart->pos().y + _btnStart->size().y + 20;
	_btnResume->setPos({ x, y });

	x = size().x / 2 - _btnConfig->size().x / 2;
	y = _btnResume->pos().y + _btnResume->size().y + 20;
	_btnConfig->setPos({ x, y });

	x = size().x / 2 - _btnExit->size().x / 2;
	y = _btnConfig->pos().y + _btnConfig->size().y + 20;
	_btnExit->setPos({ x, y });
}

}