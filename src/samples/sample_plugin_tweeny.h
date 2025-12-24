#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"

#include "tweeny/tweeny.h"

namespace samples {
    using namespace engine;

    enum EaseMode {Once, Loop, Yoyo};

    class ImFormTweenForm : public imgui::ImForm
	{
    public:
        ImFormTweenForm();
        ~ImFormTweenForm() = default;

        signals::Signal<const char*, EaseMode, int> on_select_ease;

        EaseMode getEaseMode() { return _ease_mode; }

	private:
        
        void draw() override;

        void init();

    private:
        int _select_index = 0;
        
        int _duration = 100;

        EaseMode _ease_mode = EaseMode::Once;

        std::vector<const char*> _easeAlgoText;
    };



    class SamplePluginTweeny final : public engine::Plugin, 
                                     public utility::sigslot::SlotHandler
    {
    private:
        tweeny::tween<int, int> _tween;

        Vec2i _roleBeginPos = { 100, 400 };
        Vec2i _roleEndPos = { 1100, 400 };
       
        engine::Vec2 _rolePos = _roleBeginPos;

    public:
        SamplePluginTweeny() = default;
        ~SamplePluginTweeny() = default;

        const char* name() override { return "sample_tweeny_plugin"; }

        void onInit() override;

        void onInstall() override;

        void onUninstall() override;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override;

        void onDraw() override;

        void onClose() override;

        void onEaseModeSelect(const char* ease, EaseMode mode, int duration);
    };
}