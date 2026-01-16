#include "form_loading.h"


namespace game
{

    FormLoading::FormLoading(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        root()->setBgColor(Color::DarkCyan);

        _progressBar = root()->createChild<ui::ProgressBar>("progress_loading");
        setProgress(0.0f);

        setMaximize(true);
    }

    FormLoading::~FormLoading()
    {
    }

    void FormLoading::setProgress(float progress)
    {
        _progress = std::clamp(progress, 0.0f, 1.0f);

        if (_progressBar)
        {
            _progressBar->setProgress(_progress);
        }
    }

    void FormLoading::onUpdate(float delta)
    {
        if (_progressBar)
        {
            _progressBar->setSize({ size().x - 100, 20 });
            _progressBar->setPos({ 50, size().y - 50 - 20 });
        }
    }
}