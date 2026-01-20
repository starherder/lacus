#include "form_loading.h"


namespace game
{

    FormLoading::FormLoading(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        root()->setBgColor(Color::DarkCyan);
        root()->setAcceptEvent(true);

        _progressBar = root()->createChild<ui::ProgressBar>("progress_loading");
        setProgress(0.0f);

        _textTip = root()->createChild<ui::Label>("text_tip");
        _textTip->setText("loading...");
        _textTip->setFont("fonts/Vonwaon.ttf", 40);
        _textTip->setSize({ 300, 100 });
        _textTip->setTextColor(Color::Pale);
        _textTip->setBgColor({ 0,0,0,0 });
        _textTip->setBorderColor({ 0,0,0,0 });

        setMaximize(true);
        setDragMovable(false);
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
    }

    void FormLoading::onSizeChanged()
    {
        if (_textTip)
        {
            auto x = size().x / 2 - _textTip->size().x / 2;
            auto y = size().y / 2 - 160;
            _textTip->setPos({ x, y });
        }

        if (_progressBar)
        {
            _progressBar->setSize({ size().x - 100, 20 });
            _progressBar->setPos({ 50, size().y - 50 - 20 });
        }
    }
}