#pragma once

#include "buttons.h"
#include "form.h"

namespace ui {

class Button;
class CheckBox;
class SliderBar;
class ProgressBar;

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
    void onSlideValueChanged(SliderBar* slider);
    void onProgressChanged(ProgressBar* pbar);
    void onCheckChanged(CheckBox* cbox);
};

}