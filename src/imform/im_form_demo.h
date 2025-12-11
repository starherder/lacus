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

	class ImFormTest : public ImForm
	{
	public:
		ImFormTest();
		~ImFormTest();

		void init(engine::Application* app);

	protected:
		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};
}