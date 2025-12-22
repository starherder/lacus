#include "form_demo.h"
#include "gui_manager.h"
#include "buttons.h"

namespace ui {


FormDemo::FormDemo(const std::string& name) : Form(name)
{
	setPos({ 100, 100 });
	setSize({ 1000, 600 });

	// create child
	auto group = root();
	auto bar = group->createChild<Label>("head_bar");
	bar->setPos({100, 0});
	bar->setSize({800, 50});
	bar->setBgColor({125, 200, 125, 255});
	bar->setTextPadding({300, 8});
	bar->setTextColor({0, 125, 0, 255});
	bar->setFont("fonts/msyh.ttf", 30);
	bar->setText("Wigets Demo");

	// get child
	group->createChild<Widget>("rect_area");
	auto area = group->getChild<Widget>("rect_area");
	area->setPos({100, 80});
	area->setSize({200, 100});
	area->status().groud_color = { 255, 255, 125, 255 };

	// group in group
	auto grp_right = group->createChild<Group>("grp_right");
	grp_right->setPos({100, 200});
	grp_right->setSize({300, 350});
	grp_right->setBgColor({ 125, 125, 200, 255 });

	auto ctl_red = grp_right->createChild<Widget>("ctl_red");
	ctl_red->setPos({50, 50});
	ctl_red->setSize({300, 50});
	ctl_red->setBgColor({ 255, 125, 125, 255 });

	auto texture = GuiManager::inst().textureManager().get("textures/UI/emote.png");
	auto ctl_txt = grp_right->createChild<Widget>("ctl_txt");
	ctl_txt->setPos({ 50, 150 });
	ctl_txt->setSize({ 300, 50 });
	ctl_txt->setBgColor({ 255, 255, 255, 255 });
	ctl_txt->setBorderColor({ 255, 0, 0, 255 });
	ctl_txt->setTexture(texture, {0, 0, 1.0f, 0.333f});

	auto lbl_txt = grp_right->createChild<Label>("lbl_txt");
	lbl_txt->setPos({ 50, 250 });
	lbl_txt->setSize({ 300, 50 });
	lbl_txt->setBgColor({ 125, 255, 125, 125 });
	lbl_txt->setFont("fonts/VonwaonBitmap-16px.ttf", 30);
	lbl_txt->setTextColor({0,255,0,255});
	lbl_txt->setText("hello,world");
	lbl_txt->setTextPadding({50, 10});


	auto grp_btns = group->createChild<Group>("grp_btns");
	grp_btns->setPos({ 500, 200 });
	grp_btns->setSize({ 300, 350 });
	grp_btns->setBgColor({ 125, 200, 125, 255 });

	auto btn1 = grp_btns->createChild<Button>("btn_1");
	btn1->setPos({50, 100});
	btn1->setSize({200, 50});
	btn1->setText("click me");
	btn1->setTextPadding({50, 5});
	btn1->on_click.connect(this, &FormDemo::onButtonOneClick);

}

FormDemo::~FormDemo()
{
}

void FormDemo::onUpdate(float delta) 
{

}

void FormDemo::onDraw() 
{

}


void FormDemo::onButtonOneClick(Button* btn)
{
	spdlog::info("on button ({}) click !!", btn->name());
}

}