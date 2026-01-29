#pragma once

#include "game/ui/form_logic_base.h"

namespace game 
{

    class FormChessTip : public FormLogicBase
    {
    public:
        FormChessTip(const std::string& name) = delete;
        FormChessTip(const std::string& name, GameContext& context);
        ~FormChessTip();

        void showChessTip(entt::entity obj);

    private:
        void onUpdate(float delta) override;

        void onSizeChanged() override;

    private:
        ui::Label* _lblName = nullptr;
        ui::Label* _lblLevel = nullptr;
        ui::ListBox* _listProps = nullptr;
    };
}