#pragma once

#include "engine/application.h"
#include "imform/imform_manager.h"
#include "imform/imform.h"

namespace samples {

	class ImFormLuaSample : public imgui::ImForm
	{
	public:
        ImFormLuaSample() = delete;
        ImFormLuaSample(engine::Application* app) : _application(app){}
		~ImFormLuaSample() = default;

	protected:
		void onInit() override;

		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};


    class SamplePluginLua final : public engine::Plugin 
    {
    public:
        SamplePluginLua() = default;
        ~SamplePluginLua() = default;

        const char* name() override { return "sample_lua_plugin"; }

        void onInit() override ;

        void onInstall() override ;

        void onUninstall() override ;

        void onEnable() override;

        void onDisable() override;

        void onUpdate() override ;

        void onDraw() override ;

        void onClose() override ;

        void greetingLuaBridget();
    };
}