#include "form_loading.h"


namespace game
{

    FormLoading::FormLoading(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_loading.xml");

        setProgress(0);
    }

    FormLoading::~FormLoading()
    {
    }

    void FormLoading::setProgress(float progress)
    {
        progress = std::clamp(progress, 0.0f, 1.0f);

        auto progressBar = getWidget<ProgressBar>("pro_loading");
        if (progressBar)
        {
            progressBar->setProgress(progress);
        }
    }

    void FormLoading::onUpdate(float delta)
    {
    }

    void FormLoading::onSizeChanged()
    {
    }
}