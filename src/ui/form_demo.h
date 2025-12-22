#pragma once

#include "form.h"

namespace ui {

class Button;

class FormDemo : public Form 
{
public:
    FormDemo() = delete;
    FormDemo(const std::string& name);
    virtual ~FormDemo();

    void onUpdate(float delta) override;
    void onDraw() override;

private:
    void onButtonOneClick(Button* btn);
};

}