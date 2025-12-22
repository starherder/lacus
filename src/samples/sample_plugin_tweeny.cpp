#include "sample_plugin_tweeny.h"
#include "magic_enum/magic_enum.h"

namespace samples {

    ImFormTweenForm::ImFormTweenForm() 
    { 
        init(); 
    }

    void ImFormTweenForm::draw()
    {
        ImGui::Begin("Tween");

        ImGui::TextColored({ 0, 0.5, 0, 1 }, "select ease mode:");

        ImGui::SetNextWindowSize(ImVec2(-FLT_MIN, 200.0f));
        if (ImGui::ListBox("##listbox", &_select_index, _easeAlgoText.data(), _easeAlgoText.size(), 4))
        {
            auto& sel_ease = _easeAlgoText[_select_index];
            on_select_ease.emit(sel_ease, _ease_mode, _duration);
        }

        ImGui::SliderInt("duration(ms)", &_duration, 10, 10000);

        if (ImGui::RadioButton("Once", _ease_mode == EaseMode::Once)) {
            _ease_mode = EaseMode::Once;
        }

        if (ImGui::RadioButton("Loop", _ease_mode == EaseMode::Loop)) {
            _ease_mode = EaseMode::Loop;
        }

        if (ImGui::RadioButton("Yoyo", _ease_mode == EaseMode::Yoyo)) {
            _ease_mode = EaseMode::Yoyo;
        }

        ImGui::End();
    }

    void ImFormTweenForm::init()
    {
        _easeAlgoText.push_back("stepped");
        _easeAlgoText.push_back("linear");
        _easeAlgoText.push_back("quadraticIn");
        _easeAlgoText.push_back("quadraticOut");
        _easeAlgoText.push_back("quadraticInOut");
        _easeAlgoText.push_back("cubicIn");
        _easeAlgoText.push_back("cubicOut");
        _easeAlgoText.push_back("cubicInOut");
        _easeAlgoText.push_back("quarticIn");
        _easeAlgoText.push_back("quarticOut");
        _easeAlgoText.push_back("quarticInOut");
        _easeAlgoText.push_back("quinticIn");
        _easeAlgoText.push_back("quinticOut");
        _easeAlgoText.push_back("quinticInOut");
        _easeAlgoText.push_back("sinusoidalIn");
        _easeAlgoText.push_back("sinusoidalOut");
        _easeAlgoText.push_back("sinusoidalInOut");
        _easeAlgoText.push_back("exponentialIn");
        _easeAlgoText.push_back("exponentialOut");
        _easeAlgoText.push_back("exponentialInOut");
        _easeAlgoText.push_back("circularIn");
        _easeAlgoText.push_back("circularOut");
        _easeAlgoText.push_back("circularInOut");
        _easeAlgoText.push_back("bounceIn");
        _easeAlgoText.push_back("bounceOut");
        _easeAlgoText.push_back("bounceInOut");
        _easeAlgoText.push_back("elasticIn");
        _easeAlgoText.push_back("elasticOut");
        _easeAlgoText.push_back("elasticInOut");
        _easeAlgoText.push_back("backIn");
        _easeAlgoText.push_back("backOut");
        _easeAlgoText.push_back("backInOut");
    }

    /////////////////////////////////////////////////////////////////



    void SamplePluginTweeny::onInit()
    {
    }

    void SamplePluginTweeny::onInstall()
    {
    }

    void SamplePluginTweeny::onUninstall()
    {
    }

    void SamplePluginTweeny::onEnable()
    {
        auto form = imgui::ImFormManager::inst().showForm<ImFormTweenForm>("ImFormTweenForm");
        if (form)
        {
            form->on_select_ease.connect(this, &SamplePluginTweeny::onEaseModeSelect);
        }
    }

    void SamplePluginTweeny::onDisable()
    {
        imgui::ImFormManager::inst().closeForm("ImFormTweenForm");
    }

    void SamplePluginTweeny::onUpdate()
    {
        if (!_tween.isFinished())   // 是否结束
        {
            auto delta = application()->fpsChecker().deltaTicks();
            //spdlog::info("delta = {}, time_point = {}, point = {}", 
            //    delta, _tween.currentTimePoint(), _tween.point());

            _tween.step(delta);     // 走一步（经过一点时间）
        }
    }

    void SamplePluginTweeny::onDraw()
    {
        auto& renderer = application()->renderer();

        renderer.setDrawColor({ 255, 255, 0, 255 });
        renderer.drawFillRect({ _rolePos.x, _rolePos.y, 100, 100 });
    }

    void SamplePluginTweeny::onClose()
    {
    }

    void SamplePluginTweeny::onEaseModeSelect(const char* ease, EaseMode mode, int duration)
    {
        spdlog::info("ease = {}, mode = {}", ease, MagicEnumText(EaseMode, mode));

        _tween = tweeny::from(_roleBeginPos.x, _roleBeginPos.y)  // 初始值，可以有多个
                        .to(_roleEndPos.x, _roleEndPos.y)        // 结束值，可有有多个
                        .during(duration)                        // 总时长，毫秒
                        .via(ease)                               // 算法
                        .onStep([this](auto& t, int x, int y) {  // 每一步都进入此回调
                            _rolePos.x = x;
                            _rolePos.y = y;
                            return false;
                        });

        int start_ticks = (int)SDL_GetTicks();

        if (mode == EaseMode::Once)
        {
            // 继续加回调
            _tween.onStep([ease, start_ticks](auto& t, int x, int y) {
                if (t.progress() >= 1.0f) {
                    spdlog::info("{} finish!,  ticks = {}", ease, SDL_GetTicks()-start_ticks);
                    return true;
                }
                return false;
            });
        }

        if (mode == EaseMode::Loop)
        {
            // 继续加回调
            _tween.onStep([ease](auto& t, int x, int y) {  
                if (t.progress() >= 1.0f) {
                    spdlog::info("{} reward!", ease);
                    t.seek(0); 
                }
                return false;
            });
         }

        if (mode == EaseMode::Yoyo)
        {
            // 继续加回调
            _tween.onStep([ease](auto& t, int x, int y) {
                if (t.progress() <= 0.001f) { 
                    spdlog::info("{} forward!", ease);
                    t.forward(); 
                }
                if (t.progress() >= 1.0f) {
                    spdlog::info("{} backward!", ease);
                    t.backward(); 
                }
                return false;
            });
        }
    }

}