#include "form_main.h"


namespace game 
{

FormMain::FormMain(const std::string& name) : Form(name)
{
	setPos({ 100, 100 });
	setSize({ 500, 800 });

	auto group = root();
	group->setBgColor({ 0, 255, 0, 200 });

    auto bar = group->createChild<Label>("head_bar");
    bar->setPos({ 0, 0 });
    bar->setSize({ 500, 50 });
    
    bar->setBgColor({ 125, 200, 125, 200 });
    bar->setTextColor({ 0, 125, 0, 255 });

    bar->setFont("fonts/msyh.ttf", 30);
    bar->setText("main");
}

FormMain::~FormMain()
{
}



}