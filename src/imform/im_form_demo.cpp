#include "im_form_demo.h"


namespace imgui
{

	ImFormDemo::ImFormDemo()
	{
	}
	ImFormDemo::~ImFormDemo()
	{
	}

	void ImFormDemo::draw()
	{
		ImGui::ShowDemoWindow(&_visible);
	}
}