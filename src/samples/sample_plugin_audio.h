#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"
#include "imform/imform.h"

namespace samples {

	class ImGuiFormAudio : public imgui::ImForm
	{
	public:
		ImGuiFormAudio() = delete;
		ImGuiFormAudio(engine::Application* app) : _application(app){}
		~ImGuiFormAudio() = default;

	protected:
		void init();
		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};


    class SamplePluginAudio final : public engine::Plugin 
    {
    public:
        SamplePluginAudio() = default;
        ~SamplePluginAudio() = default;

        const char* name() override { return "sample_audio_plugin"; }

        void onInit() override ;

        void onInstall() override ;

        void onUninstall() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;

        void onClose() override ;

    };
}