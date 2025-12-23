#include "form_demo.h"
#include "groups.h"
#include "gui_manager.h"
#include "buttons.h"

namespace ui {


FormDemo::FormDemo(const std::string& name) : Form(name)
{
	setPos({ 100, 100 });
	setSize({ 1200, 800 });

	// create child
	auto group = root();

	{// ---------------------- base ----------------------
		auto bar = group->createChild<Label>("head_bar");
		bar->setPos({100, 0});
		bar->setSize({1000, 50});
		bar->setBgColor({125, 200, 125, 255});
		//bar->setTextPadding({300, 8});
		bar->setTextColor({0, 125, 0, 255});
		bar->setFont("fonts/msyh.ttf", 30);
		bar->setText("Wigets Demo");

		// get child
		group->createChild<Widget>("rect_area");
		auto area = group->getChild<Widget>("rect_area");
		area->setPos({100, 80});
		area->setSize({50, 50});
		area->setBgColor({ 255, 255, 125, 255 });
	}


	{ // ---------------------- group ----------------------
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
		//lbl_txt->setTextPadding({50, 10});
	}

	{// ---------------------- buttons ----------------------
		auto grp_btns = group->createChild<Group>("grp_btns");
		grp_btns->setPos({ 500, 200 });
		grp_btns->setSize({ 300, 300 });
		grp_btns->setBgColor({ 125, 200, 125, 255 });

		auto btn1 = grp_btns->createChild<Button>("btn_1");
		btn1->setPos({10, 10});
		btn1->setSize({200, 50});
		btn1->setText("click me");
		//btn1->setTextPadding({50, 5});
		btn1->on_click.connect(this, &FormDemo::onButtonOneClick);

		auto sld1 = grp_btns->createChild<SliderBar>("sld_1");
		sld1->setPos({10, 80});
		sld1->setSize({200, 30});
		sld1->setValue(55);
		sld1->setMaxValue(150);
		sld1->on_value_changed.connect(this, &FormDemo::onSlideValueChanged);
		
		auto sld2 = grp_btns->createChild<SliderBar>("sld_2");
		sld2->setPos({220, 10});
		sld2->setSize({30, 150});
		sld2->setValue(15);
		sld2->setMaxValue(100);
		sld2->setDirection(Coordinate::Vertical);
		sld2->on_value_changed.connect(this, &FormDemo::onSlideValueChanged);

		auto pbar1 = grp_btns->createChild<ProgressBar>("pbar_1");
		pbar1->setPos({10, 150});
		pbar1->setSize({200, 30});
		pbar1->setProgress(0.5f);
		pbar1->on_process_changed.connect(this, &FormDemo::onProgressChanged);
		
		auto pbar2 = grp_btns->createChild<ProgressBar>("pbar_2");
		pbar2->setPos({220, 180});
		pbar2->setSize({30, 150});
		pbar2->setProgress(0.3f);
		pbar2->setDirection(Coordinate::Vertical);
		pbar2->on_process_changed.connect(this, &FormDemo::onProgressChanged);
	}

	{	// ---------------------- horizonal layout ----------------------
		auto hlay = root()->createChild<HorizonalLayout>("hlay");
		hlay->setPos({ 100, 650 });
		hlay->setSize({ 800, 100 });
		hlay->setBgColor(Color::LightRed);

			auto wgt = hlay->createChild<Widget>("wgt_1");
			wgt->setSize({ 50, 50 });
			wgt->setBgColor(Color::LightBlue);

			auto btn = hlay->createChild<Button>("btn_2");
			btn->setSize({ 200, 50 });
			btn->setText("click me");
			//btn->setTextPadding({ 50, 5 });
			btn->on_click.connect(this, &FormDemo::onButtonOneClick);

			auto lbl = hlay->createChild<Label>("lbl_3");
			lbl->setText("hello,world");
			lbl->setTextAlign(Align::Left);
			lbl->setTextPadding({ 10, 5 });
	}

	{	// ---------------------- vertical layout ----------------------
		auto vlay = root()->createChild<VerticalLayout>("vlay");
		vlay->setPos({ 900, 100 });
		vlay->setSize({ 100, 600 });
		vlay->setBgColor(Color::LightGreen);
			auto wgt = vlay->createChild<Widget>("wgt_3");
			wgt->setSize({ 50, 50 });
			wgt->setBgColor(Color::LightRed);

			auto btn = vlay->createChild<Button>("btn_3");
			btn->setSize({ 50, 200 });
			btn->setText("click me");
			//btn->setTextPadding({ 5, 50 });
			btn->on_click.connect(this, &FormDemo::onButtonOneClick);

			auto lbl = vlay->createChild<Label>("lbl_3");
			lbl->setText("hello,world");
			//lbl->setTextPadding({ 50, 5 });
	}
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

void FormDemo::onProgressChanged(ProgressBar* pbar)
{
	spdlog::info("on progress ({}) changed to {}", pbar->name(), pbar->progress());
}

void FormDemo::onSlideValueChanged(SliderBar* bar)
{
	spdlog::info("on slide ({}) changed to {}/{}", bar->name(), bar->value(), bar->maxValue());
}

}