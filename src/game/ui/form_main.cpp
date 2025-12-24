#include "form_main.h"


namespace game 
{

FormMain::FormMain(const std::string& name) : Form(name)
{
	setPos({ 100, 100 });
	setSize({ 500, 500 });

	auto grp_root = root();
	grp_root->setBgColor({ 0, 255, 0, 200 });

    auto vlay = grp_root->createChild<VerticalLayout>("vlay");
    vlay->setPos({ 0, 0 });
    vlay->setSize({ 500, 500 });

    auto bar = vlay->createChild<Label>("head_bar");
    bar->setSize({ 480, 40 });
    bar->setBgColor({ 125, 200, 125, 200 });
    bar->setTextColor({ 0, 125, 0, 255 });
    bar->setText("main");

    auto hlay1 = vlay->createChild<HorizonalLayout>("hlay1");
    hlay1->setSize({480, 360});
    hlay1->setBgColor({ 0, 125, 0, 200 });

    auto hlay2 = vlay->createChild<HorizonalLayout>("hlay2");
    hlay2->setBgColor({ 0, 255, 125, 200 });
    {
        auto btn_test = hlay2->createChild<Button>("test");
        btn_test->setText("test");
        btn_test->on_click.connect(this, &FormMain::onTestClick);
        
        auto btn_run = hlay2->createChild<Button>("run");
        btn_run->setText("run");
        btn_run->on_click.connect(this, &FormMain::onRunClick);
    }

}

FormMain::~FormMain()
{
}

void FormMain::onTestClick(Button* btn)
{
    spdlog::info("on test click");
}

void FormMain::onRunClick(Button* btn)
{
    spdlog::info("on run click");
}

}