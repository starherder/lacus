#pragma once

#include "im_form.h"
#include <filesystem>

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
		ImFormTest() = default;
		~ImFormTest() = default;

	protected:
		void draw() override;
	};
}