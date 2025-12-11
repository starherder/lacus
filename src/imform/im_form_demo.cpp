#include "im_form_demo.h"


namespace imgui
{
	void ImFormDemo::draw()
	{
		ImGui::ShowDemoWindow(&_visible);
	}

	void ImFormTest::draw()
	{
		ImGui::Begin("长风几万里");
		{
			if (ImGui::Button("能饮一杯无")) {
			}

			if (ImGui::Button("邀我至田家")){
			}

			if (ImGui::Button("何不带吴钩")){
			}
		}
		ImGui::End();
	}
}


