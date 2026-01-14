#include "sample_plugin_ui.h"

#include "engine/application.h"
#include "engine/color.h"
#include "ui/game_widgets.h"

namespace samples {


	FormCards::FormCards(const std::string& name) : Form(name)
	{
		setPos({ 100, 100 });
		setSize({ 1500, 1000 });

		// create child
		auto bgGroup = root();
		bgGroup->setBgColor({ 0, 128, 0, 200});

		{
			auto group = bgGroup->createChild<ui::Group>("test_group");
			group->setSize({ 350, 350 });
			group->setPos({ 20, 20 });
			group->setBgColor({ 128, 128, 0, 200 });

			for (int i = 0; i < 5; i++) {
				auto label = group->createChild<ui::Group>(fmt::format("lbl_{}", i));
				label->setSize({80, 80});
				label->setBgColor({i*10, 255-i*10, i*5});
				label->setPos({i*80, i*80});
			}
		}

		{
			auto group = bgGroup->createChild<ui::CardGroup>("card_group");
			group->setSize({ size().x, 350 });
			group->setPos({ 0, size().y - group->size().y });
			group->setBgColor({ 0, 128, 0, 200 });

			group->addCard("test0");
			group->addCard("test1");
			group->addCard("test2");
			group->addCard("test3");
			group->addCard("test4");
			group->addCard("test5");
			group->addCard("test6");
			group->addCard("test7");
			group->addCard("test8");
			group->addCard("test9");
		}

		auto cbox = root()->createChild<ui::CheckBox>("cbox_overlap");
		cbox->setSize({ 100, 50 });
		cbox->setPos({ 1400, 0 });
		cbox->setText("overlap");
		cbox->on_check_changed.connect(this, &FormCards::onCardOverlapChanged);

		ui::GuiManager::inst().on_drop.connect(this, &FormCards::onDropCard);
	}

	FormCards::~FormCards()
	{
	}

	void FormCards::onCardOverlapChanged(ui::CheckBox* cb)
	{
		auto group = getWidget<ui::CardGroup>("card_group");
		if (group) {
			group->setOverlap(cb->checked());
		}
	}

	void FormCards::onDropCard(ui::Widget* widget, const Vec2& pos)
	{
		auto dragging = ui::GuiManager::inst().fetchDraggingData();
		if (!dragging) return;

		auto cardWidget = dragging->widget;
		auto sourceGroup = dynamic_cast<ui::CardGroup*>(dragging->source);
		if (!cardWidget || !sourceGroup)
		{
			spdlog::error("dragging card widget is invalid.");
			return;
		}

		spdlog::info("drop card ({}) at widget ({}), at pos ({},{})",
			cardWidget->name(), widget?widget->name():"none", pos.x, pos.y);

		if (sourceGroup) 
		{
			int index = cardWidget->getData<int>("index");
			sourceGroup->addWidget(cardWidget, index);
		}

	}

	void FormCards::onUpdate(float delta)
	{

	}

	void FormCards::onDraw()
	{

	}

	/////////////////////////////////////////////////////////////////////////////////



	FormDemo::FormDemo(const std::string& name, SamplePluginUI* plugin) : Form(name), _plugin(plugin)
	{
		setPos({ 100, 100 });
		setSize({ 1500, 1000 });

		// create child
		auto group = root();
		group->setBgColor({ 0, 100, 100, 200 });

		{// ---------------------- base ----------------------
			auto bar = group->createChild<ui::Label>("head_bar");
			bar->setPos({ 100, 0 });
			bar->setSize({ 1300, 50 });
			bar->setBgColor({ 125, 200, 125, 255 });
			//bar->setTextPadding({300, 8});
			bar->setTextColor({ 0, 125, 0, 255 });
			bar->setFont("fonts/msyh.ttf", 30);
			bar->setText("Wigets Demo");

			// get child
			group->createChild<ui::Widget>("rect_area");
			auto area = group->getChild<ui::Widget>("rect_area");
			area->setPos({ 100, 80 });
			area->setSize({ 50, 50 });
			area->setBgColor({ 255, 255, 125, 255 });
		}

		{ // ---------------------- group ----------------------
			auto grp_right = group->createChild<ui::Group>("grp_right");
			grp_right->setPos({ 100, 200 });
			grp_right->setSize({ 300, 350 });
			grp_right->setBgColor({ 125, 125, 200, 255 });

			auto ctl_red = grp_right->createChild<ui::Widget>("ctl_red");
			ctl_red->setPos({ 50, 50 });
			ctl_red->setSize({ 300, 50 });
			ctl_red->setBgColor({ 255, 125, 125, 255 });

			auto texture = ui::GuiManager::inst().textureManager().get("textures/UI/emote.png"_hs);
			auto ctl_txt = grp_right->createChild<ui::Widget>("ctl_txt");
			ctl_txt->setPos({ 50, 150 });
			ctl_txt->setSize({ 300, 50 });
			ctl_txt->setBgColor({ 255, 255, 255, 255 });
			ctl_txt->setBorderColor({ 255, 0, 0, 255 });
			ctl_txt->setTexture(texture, { 0.0f, 0.0f, 1.0f, 0.333f });

			auto lbl_txt = grp_right->createChild<ui::Label>("lbl_txt");
			lbl_txt->setPos({ 50, 250 });
			lbl_txt->setSize({ 300, 50 });
			lbl_txt->setBgColor({ 125, 255, 125, 125 });
			lbl_txt->setFont("fonts/VonwaonBitmap-16px.ttf", 30);
			lbl_txt->setTextColor({ 0,255,0,255 });
			lbl_txt->setText("hello,world");
			//lbl_txt->setTextPadding({50, 10});
		}

		{// ---------------------- buttons ----------------------
			auto grp_btns = group->createChild<ui::Group>("grp_btns");
			grp_btns->setPos({ 500, 200 });
			grp_btns->setSize({ 300, 350 });
			grp_btns->setBgColor({ 125, 200, 125, 255 });

			auto btn1 = grp_btns->createChild<ui::Button>("btn_1");
			btn1->setPos({ 10, 10 });
			btn1->setSize({ 200, 50 });
			btn1->setText("click me");
			//btn1->setTextPadding({50, 5});
			btn1->on_click.connect(this, &FormDemo::onButtonOneClick);

			auto sld1 = grp_btns->createChild<ui::SliderBar>("sld_1");
			sld1->setPos({ 10, 80 });
			sld1->setSize({ 200, 30 });
			sld1->setValue(55);
			sld1->setMaxValue(150);
			sld1->on_value_changed.connect(this, &FormDemo::onSlideValueChanged);

			auto sld2 = grp_btns->createChild<ui::SliderBar>("sld_2");
			sld2->setPos({ 220, 10 });
			sld2->setSize({ 30, 150 });
			sld2->setValue(15);
			sld2->setMaxValue(100);
			sld2->setDirection(ui::Coordinate::Vertical);
			sld2->on_value_changed.connect(this, &FormDemo::onSlideValueChanged);

			auto pbar1 = grp_btns->createChild<ui::ProgressBar>("pbar_1");
			pbar1->setPos({ 10, 150 });
			pbar1->setSize({ 200, 30 });
			pbar1->setProgress(0.5f);
			pbar1->on_process_changed.connect(this, &FormDemo::onProgressChanged);

			auto pbar2 = grp_btns->createChild<ui::ProgressBar>("pbar_2");
			pbar2->setPos({ 220, 180 });
			pbar2->setSize({ 30, 150 });
			pbar2->setProgress(0.3f);
			pbar2->setDirection(ui::Coordinate::Vertical);
			pbar2->on_process_changed.connect(this, &FormDemo::onProgressChanged);

			auto chk1 = grp_btns->createChild<ui::CheckBox>("chk_1");
			chk1->setPos({ 10, 220 });
			chk1->setSize({ 200, 30 });
			chk1->setText("check me");
			chk1->setChecked(true);
			chk1->on_check_changed.connect(this, &FormDemo::onCheckChanged);
		}

		{	// ---------------------- horizonal layout ----------------------
			auto hlay = root()->createChild<ui::HorizonalLayout>("hlay");
			hlay->setPos({ 100, 650 });
			hlay->setSize({ 800, 100 });
			hlay->setBgColor(Color::LightRed);

			auto wgt = hlay->createChild<ui::Widget>("wgt_1");
			wgt->setSize({ 50, 50 });
			wgt->setBgColor(Color::LightBlue);

			auto btn = hlay->createChild<ui::Button>("btn_2");
			btn->setSize({ 200, 50 });
			btn->setText("click me");
			//btn->setTextPadding({ 50, 5 });
			btn->on_click.connect(this, &FormDemo::onButtonOneClick);

			auto lbl = hlay->createChild<ui::Label>("lbl_3");
			lbl->setText("hello,world");
			lbl->setTextAlign(ui::Align::Left);
			lbl->setTextPadding({ 10, 5 });
		}

		{	// ---------------------- vertical layout ----------------------
			auto vlay = root()->createChild<ui::VerticalLayout>("vlay");
			vlay->setPos({ 900, 100 });
			vlay->setSize({ 100, 600 });
			vlay->setBgColor(Color::LightGreen);
			auto wgt = vlay->createChild<ui::Widget>("wgt_3");
			wgt->setSize({ 50, 50 });
			wgt->setBgColor(Color::LightRed);

			auto btn = vlay->createChild<ui::Button>("btn_3");
			btn->setSize({ 50, 200 });
			btn->setText("click me");
			//btn->setTextPadding({ 5, 50 });
			btn->on_click.connect(this, &FormDemo::onButtonOneClick);

			auto lbl = vlay->createChild<ui::Label>("lbl_3");
			lbl->setText("hello,world");
			//lbl->setTextPadding({ 50, 5 });
		}

		{
			auto exgroup = root()->createChild<ui::ExpandGroup>("exp_group");
			exgroup->setPos({1050, 100});
			exgroup->setSize({300, 300});

			for (int i = 0; i < 20; i++)
			{
				std::string btnname = fmt::format("btn_{}", i);
				auto item = exgroup->createChild<ui::Button>(btnname);
				item->setPos({rand()%500, rand()%500});
				item->setSize({100, 60});	
				item->setText(btnname);
				item->setData("index", i);
				item->on_click.connect([](ui::Button* btn) {
					int index = btn->getData<int>("index");
					spdlog::info("click: index = {}", index);
				});
			}
		}

		{
			auto radioHGroup = root()->createChild<ui::RadioHLayGroup>("radio_hlay_group");
			radioHGroup->setPos({1050, 450});
			radioHGroup->setSize({ 300, 80 });
			radioHGroup->addItem("alpha");
			radioHGroup->addItem("belta");
			radioHGroup->addItem("gama");
			radioHGroup->on_item_select.connect([](int index) {
				spdlog::info("radio horizonal group: index: {} selected", index);
			});

			auto radioVGroup = root()->createChild<ui::RadioVLayGroup>("radio_vlay_group");
			radioVGroup->setPos({ 1050, 550 });
			radioVGroup->setSize({ 300, 180 });
			radioVGroup->addItem("alpha");
			radioVGroup->addItem("belta");
			radioVGroup->addItem("gama");
			radioVGroup->on_item_select.connect([](int index) {
				spdlog::info("radio vertical group: index: {} selected", index);
			});

			auto listbox = root()->createChild<ui::ListBox>("list_box");
			listbox->setPos({1050, 750});
			listbox->setSize({300, 180});
			listbox->on_item_select.connect([this](int index) { on_list_select.emit(index); });
			for(int i=0; i<20; i++) {
				listbox->addItem(fmt::format("item_{}", i));
			}
		}

		{
			auto btn = root()->createChild<ui::Button>("btn_show_cards");
			btn->setSize({150, 50});
			btn->setPos({200, 80});
			btn->setText("cards");
			btn->on_click.connect([](ui::Button* btn) {
				bool visible = btn->getData<bool>("cards_show");
				if (!visible) {
					ui::GuiManager::inst().showForm<FormCards>("form_cars");
				}
				else {
					ui::GuiManager::inst().closeForm("form_cars");
				}

				btn->setData("cards_show", !visible);
			});

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

	void FormDemo::onButtonOneClick(ui::Button* btn)
	{
		spdlog::info("on button ({}) click !!", btn->name());
	}

	void FormDemo::onProgressChanged(ui::ProgressBar* pbar)
	{
		spdlog::info("on progress ({}) changed to {}", pbar->name(), pbar->progress());
	}

	void FormDemo::onSlideValueChanged(ui::SliderBar* bar)
	{
		spdlog::info("on slide ({}) changed to {}/{}", bar->name(), bar->value(), bar->maxValue());

		float ratio = bar->value() / bar->maxValue();
		auto group = (ui::Group*)bar->parent();

		auto pbar1 = group->getChild<ui::ProgressBar>("pbar_1");
		if (pbar1) pbar1->setProgress(ratio);

		auto pbar2 = group->getChild<ui::ProgressBar>("pbar_2");
		if (pbar2) pbar2->setProgress(ratio);
	}

	void FormDemo::onCheckChanged(ui::CheckBox* cbox)
	{
		bool checked = cbox->checked();
		if (checked) {
			cbox->setText("checked!");
		}
		else {
			cbox->setText("check me");
		}

		spdlog::info("on check ({}) changed to {}", cbox->name(), cbox->checked());
	}

	void FormDemo::onClickListButton(ui::Button* btn)
	{
		int index = btn->getData<int>("index");
		on_list_select.emit(index);
	}

	/////////////////////////////////////////////////////////////////////////////////


    void SamplePluginUI::onInit() 
    {
    }

    void SamplePluginUI::onInstall() 
    {  
    }

    void SamplePluginUI::onUninstall() 
    {
    }

    void SamplePluginUI::onEnable()
    {
		auto form = ui::GuiManager::inst().showForm<FormDemo>("form_demo", this);
		form->on_list_select.connect(this, &SamplePluginUI::onSelectBackGroud);

		auto tex = _application.resourceManager().textureManager().get("textures/battle_of_tiles/etc.png"_hs);

		_textures.clear();
		_textures.push_back({ tex, {240, 0, 80, 80} });
		_textures.push_back({ tex, {80, 160, 80, 80} });
		_textures.push_back({ tex, {243, 3, 74, 74} });
		_textures.push_back({ tex, {83, 163, 74, 74} });

		_textures.push_back({ tex, {3, 83, 74, 74} });
		_textures.push_back({ tex, {83, 83, 74, 74} });
		_textures.push_back({ tex, {3, 163, 74, 74} });

		_textures.push_back({ tex, {3, 3, 74, 74} });
		_textures.push_back({ tex, {83, 3, 74, 74} });
    }

    void SamplePluginUI::onDisable()
    {
		ui::GuiManager::inst().closeForm("form_demo");
    }

    void SamplePluginUI::onUpdate() 
    {
		auto delta = _application.frameTicker().deltaSeconds();
		ui::GuiManager::inst().update(delta);
    }

    void SamplePluginUI::onDraw() 
    {
		auto tex = _textures[_bgIndex];
			
		for (int x = 0; x < 50; x++)
		{
			for (int y = 0; y < 50; y++)
			{
				Rect rect{x*tex.rect.w, y*tex.rect.h, tex.rect.w, tex.rect.h};
				_application.renderer().drawTexture(tex.texture, tex.rect, rect);
			}
		}
		
		ui::GuiManager::inst().draw();
    }

    void SamplePluginUI::onClose() 
    {
    }

	void SamplePluginUI::onSelectBackGroud(int index)
	{
		if (index >= _textures.size()) return;

		_bgIndex = index;
	}

}