#pragma once

#include "im_form.h"
#include <filesystem>

namespace engine{
	class Application;
}

namespace imgui
{
	// ImGui 提供的Demo
	class ImFormDemo : public ImForm
	{
	public:
		ImFormDemo() = default;
		~ImFormDemo() = default;

	protected:
		void draw() override;
	};

	class ImFormAudio : public ImForm
	{
	public:
		ImFormAudio() = default;
		~ImFormAudio() = default;

		void init(engine::Application* app);

	protected:
		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};

	class ImFormHUD : public ImForm
	{
	public:
		ImFormHUD() = default;
		ImFormHUD(engine::Application* app) : _application(app) {}
		~ImFormHUD() = default;

	protected:
		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};

	
}