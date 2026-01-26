#pragma once

#include "engine/application.h"

#include "ui/ui.h"
#include "imform/imform.h"
#include "imform/imform_manager.h"

namespace samples 
{
    using namespace engine;

    class ImFormUIViewer : public imgui::ImForm
    {
    public:
        ImFormUIViewer() = delete;
        ImFormUIViewer(engine::Application* app) : _application(app) {}
        ~ImFormUIViewer() = default;

        void setSkinPath(const fs::path& skinPath);

    protected:
        void onInit() override;

        void draw() override;

    private:
        engine::Application* _application = nullptr;

        fs::path _uiSkinPath;

        std::vector<std::string> _formNames;
        std::vector<const char*> _formList;
    };


    class FormDemo : public ui::Form
    {
    public:
        utility::sigslot::Signal<int> on_list_select;

    public:
        FormDemo() = delete;
        FormDemo(const std::string& name, Application& app);
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
        Application& _application;
    };


    class FormLayout : public ui::Form
    {
    public:
        FormLayout() = delete;
        FormLayout(const std::string& name);
        virtual ~FormLayout();

        void onUpdate(float delta) override;
        void onDraw() override;

    private:
        void onPosChanged() override;
        void onSizeChanged() override;
    };


    class FormCards : public ui::Form
    {
    public:
        FormCards() = delete;
        FormCards(const std::string& name);
        virtual ~FormCards();

        void onUpdate(float delta) override;
        void onDraw() override;

    private:
        void onCardOverlapChanged(ui::CheckBox* cb);
        void onDropCard(ui::GuiManager::DraggingPtr dragging);
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