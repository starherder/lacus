#pragma once

#include "ui/ui.h"

namespace game {
    using namespace ui;

    class FormMain : public ui::Form
    {
    public:
        FormMain() = delete;
        FormMain(const std::string& name);
        ~FormMain();

    private:
        void onTestClick(Button* btn);
        void onRunClick(Button* btn);
    };
}