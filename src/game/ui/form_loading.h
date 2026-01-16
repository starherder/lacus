#pragma once

#include "ui/ui.h"

namespace game 
{
    using namespace ui;

    class FormLoading : public ui::Form
    {
    public:
        FormLoading() = delete;
        FormLoading(const std::string& name);
        ~FormLoading();

        void setProgress(float progress);

    private:
        void onUpdate(float delta) override;

    private:
        ui::ProgressBar* _progressBar = nullptr;

        float _progress = 0.0f;
    };
}