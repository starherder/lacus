#pragma once

#include "im_form.h"
#include <filesystem>

namespace imgui
{
	// ImGui 提供的Demo
	class ImFormDemo : public ImForm
	{
	public:
		ImFormDemo();
		~ImFormDemo();

	protected:
		void draw();
	};

}