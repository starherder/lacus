#pragma once

#include "ui/ui.h"

namespace game 
{
    using namespace ui;

    class FormScenes : public ui::Form
    {
    public:
        FormScenes() = delete;
        FormScenes(const std::string& name);
        ~FormScenes();

    private:
        void onUpdate(float delta) override;

    private:
        void onCloseClick(Button* btn);

    };
}