#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "utility/signalslot.h"

namespace imgui
{
	class ImForm : public utility::sigslot::SlotHandler
	{
		friend class ImGuiManager;

	public:
		ImForm();
		virtual ~ImForm();

		void show(bool visible);
		void close();

		bool getVisible();

		void setName(const std::string& name);
		std::string getName();

	protected:
		void drawForm();

		virtual void onShow();
		virtual void draw() = 0;

		bool getPendingDestroy();

		bool _visible = true;

	private:
		std::string _name;
		bool _pending_destroy = false;
	};

	using ImFormSharePtr = std::shared_ptr<ImForm>;
}