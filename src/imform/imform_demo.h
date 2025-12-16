#pragma once

#include "imform.h"
#include <filesystem>

namespace engine{
	class Application;
}

namespace imgui
{
	// ImGui 提供的Demo
	class ImGuiFormDemo : public ImForm
	{
	public:
		ImGuiFormDemo() = default;
		~ImGuiFormDemo() = default;

	protected:
		void draw() override;
	};

	class ImGuiFormAudio : public ImForm
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

	class ImGuiFormHUD : public ImForm
	{
	public:
		ImGuiFormHUD() = default;
		ImGuiFormHUD(engine::Application* app) : _application(app) {}
		~ImGuiFormHUD() = default;

	protected:
		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};

	
}
