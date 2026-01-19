#include "form_scenes.h"
#include "game/ui/ui_logic_events.h"


namespace game 
{

FormScenes::FormScenes(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
    root()->setBgColor({ 0,100,0, 200 });
    root()->setMovable(false);
    root()->setAcceptEvent(true);

    _btnClose = root()->createChild<ui::Button>("btn_close");
    _btnClose->setPos({ size().x - 150, 50 });
    _btnClose->setSize({ 100, 50 });
    _btnClose->setText("close");
    _btnClose->on_click.connect([this](ui::Button* btn){ close(); });

    int index = 0;

    std::vector<std::string> btnSceneNames = {"scenes/demos/one.tmj", "scenes/demos/two.tmj", "scenes/demos/three.tmj"};

    std::vector<Vec2> btnScenesPos = { {300,600}, {500,100}, {900, 300} };
    for (auto& pos : btnScenesPos)
    {
        auto btn = root()->createChild<ui::Button>(std::format("btn_scene_{}", index));
        btn->setPos(pos);
        btn->setSize({100, 100});
        btn->setText(std::format("scene-{}", index));
        btn->setData("index", index);
        btn->setData("scene", btnSceneNames[index]);
        btn->on_click.connect(this, &FormScenes::onSelectScene);
        index++;
    }

    setMaximize(true);
    setDragMovable(false);
}

FormScenes::~FormScenes()
{
}

void FormScenes::onUpdate(float delta)
{
}

void FormScenes::onSizeChanged()
{
    _btnClose->setPos({ size().x - 150, 50 });
    _btnClose->setSize({ 100, 50 });
}

void FormScenes::onSelectScene(ui::Button* btn)
{
    ui::GuiManager::inst().closeForm("form_scenes");

    int index = btn->getData<int>("index");
    auto name = btn->getData<std::string>("scene");

    ui::GuiManager::inst().emitCustomEvent(Event_SelectScene, { index, name });
}
}