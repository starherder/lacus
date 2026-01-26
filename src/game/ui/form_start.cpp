#include "form_start.h"
#include "form_config.h"

namespace game 
{

FormStart::FormStart(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	auto group = root()->createChild<Group>("vlay_bg");
	group->setBgColor(Color::Light);

	_lblTitle = group->createChild<Label>("title");
	_lblTitle->setFont("fonts/Vonwaon.ttf", 40);
	_lblTitle->setBgColor(Color::Light);
	_lblTitle->setBorderColor(Color::Light);
	_lblTitle->setTextColor(Color::DarkGreen);
	_lblTitle->setText("Lacus");
	_lblTitle->setSize({ 300, 100 });
	
	_btnStart = group->createChild<ui::Button>("btn_start");
	_btnStart->setText("start");
	_btnStart->setSize({ 200, 50 });
	_btnStart->on_click.connect([this](ui::Button* btn) { onStart(btn); });

	_btnResume = group->createChild<ui::Button>("btn_resume");
	_btnResume->setText("resume");
	_btnResume->setSize({ 200, 50 });
	_btnResume->on_click.connect([this](ui::Button* btn) { onResume(btn); });

	_btnConfig = group->createChild<ui::Button>("btn_config");
	_btnConfig->setText("config");
	_btnConfig->setSize({ 200, 50 });
	_btnConfig->on_click.connect([this](ui::Button* btn) { onConfig(btn); });

	_btnExit = group->createChild<ui::Button>("btn_exit");
	_btnExit->setText("exit");
	_btnExit->setSize({ 200, 50 });
	_btnExit->on_click.connect([this](ui::Button* btn) { onExit(btn); });

	setMaximize(true);
	setDragMovable(false);
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
	on_resume_game.emit();
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
	SPDLOG_INFO("FormStart::onWindowResized");

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