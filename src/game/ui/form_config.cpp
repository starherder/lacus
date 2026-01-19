#include "form_config.h"


namespace game
{

    FormConfig::FormConfig(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        root()->setBgColor(Color::DarkCyan);

        root()->setMaxChildren(true);

        _vlay_bg = root()->createChild<ui::VLayout>("vlay_main");
        _vlay_bg->setBgColor(Color::DarkCyan);

        auto hlay_title = _vlay_bg->createChild<ui::HLayout>("hlay_title");
        hlay_title->setSize({ 0, 50});

        hlay_title->createChild<Widget>("space_0");

        auto btn_close = hlay_title->createChild<ui::Button>("btn_close");
        btn_close->setText("X");
        btn_close->setSize({100, 0});
        btn_close->on_click.connect(this, &FormConfig::onBtnClose);

        auto widget = _vlay_bg->createChild<ui::Widget>("wdget_blank");
        //widget->setSize({0, 200});

        auto hlay_func = _vlay_bg->createChild<ui::HLayout>("hlay_funcs");
        hlay_func->setSize({0, 50});

        auto btn_confirm = hlay_func->createChild<ui::Button>("btn_confirm");
        btn_confirm->setSize({100, 0});
        btn_confirm->setText("confirm");
        btn_confirm->on_click.connect([this](ui::Button* btn) { close(); });

        hlay_func->createChild<ui::Widget>("black");

        auto btn_cancel = hlay_func->createChild<ui::Button>("btn_cancel");
        btn_cancel->setSize({ 100, 0 });
        btn_cancel->setText("cancel");
        btn_cancel->on_click.connect([this](ui::Button* btn) { close(); });

        setMaximize(true);
        setDragMovable(false);
    }

    FormConfig::~FormConfig()
    {
    }

    void FormConfig::onBtnClose(ui::Button* btn)
    {
        close();
    }

    void FormConfig::onUpdate(float delta)
    {
    }

    void FormConfig::onSizeChanged()
    {
        _vlay_bg->setPos({ 0, 0 });
        _vlay_bg->setSize(size());
    }
}