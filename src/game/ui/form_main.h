#pragma once

#include "ui/ui.h"

namespace game {
    using namespace ui;

    class FormMain : public Form
    {
    public:
        FormMain() = delete;
        FormMain(const std::string& name);
        ~FormMain();
    };
}