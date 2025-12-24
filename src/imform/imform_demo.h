#pragma once

#include "imform/imform.h"

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

	
}
