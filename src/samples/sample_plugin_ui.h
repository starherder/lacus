#pragma once

#include "engine/application.h"

#include "ui/ui.h"

namespace samples {

    using namespace engine;

    class FormDemo : public ui::Form
    {
    public:
        utility::sigslot::Signal<int> on_list_select;

    public:
        FormDemo() = delete;
        FormDemo(const std::string& name, class SamplePluginUI* plugin);
        virtual ~FormDemo();

        void onUpdate(float delta) override;
        void onDraw() override;

    private:
        void onButtonOneClick(ui::Button* btn);
        void onSlideValueChanged(ui::SliderBar* slider);
        void onProgressChanged(ui::ProgressBar* pbar);
        void onCheckChanged(ui::CheckBox* cbox);
        void onClickListButton(ui::Button* btn);

    private:
        class SamplePluginUI* _plugin = nullptr;
    };


    class FormCards : public ui::Form
    {
    public:
        FormCards() = delete;
        FormCards(const std::string& name);
        virtual ~FormCards();

        void setCardOverlap(bool overlap);

        void onUpdate(float delta) override;
        void onDraw() override;
    };


    class SamplePluginUI final : public engine::Plugin, public utility::sigslot::SlotHandler
    {
    public:
        SamplePluginUI() = delete;

        SamplePluginUI(Application& app) : _application(app) {}
        ~SamplePluginUI() {};

        const char* name() override { return "sample_ui_plugin"; }

        void onInit() override ;

        void onInstall() override ;

        void onUninstall() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;

        void onClose() override ;

        void onSelectBackGroud(int index);

    private:
        Application& _application;

        struct BgTexture {
            class Texture* texture;
            Rect rect;
        };

        std::vector<BgTexture> _textures;

        int _bgIndex = 0;
    };
}