#include "imform_manager.h"

#include "engine/wrapper.h"


namespace imgui
{
	ImFormManager::ImFormManager()
	{
	}

	ImFormManager::~ImFormManager()
	{
		quit();
	}

	void ImFormManager::pendingDestroyForm()
	{
		for (auto it = _forms.begin(); it != _forms.end(); )
		{
			auto pForm = it->second;
			if (pForm && pForm->getPendingDestroy())
			{
				_forms.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	void ImFormManager::closeForm(const std::string& name)
	{
		auto pForm = getForm<ImForm>(name);
		if (pForm)
		{
			pForm->close();
		}
	}

	void ImFormManager::draw()
	{
		beforeDraw();

		pendingDestroyForm();

		for (const auto& [name, pForm] : _forms)
		{
			if (pForm)
			{
				pForm->drawForm();
			}
		}

		afterDraw();
	}

	void ImFormManager::beforeDraw()
	{
#ifndef USE_IMGUI_AS_RENDER_ENGINE
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
#endif
	}

	void ImFormManager::afterDraw()
	{
#ifndef USE_IMGUI_AS_RENDER_ENGINE
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);
#endif
	}

	void ImFormManager::init(struct SDL_Window* window, struct SDL_Renderer* renderer)
	{
        _window = window;
        _renderer = renderer;
		assert(_renderer);

#ifndef USE_IMGUI_AS_RENDER_ENGINE
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

        // 初始化 ImGui 的 SDL3 和 SDL_Renderer3 后端
        ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer);
        ImGui_ImplSDLRenderer3_Init(_renderer);
#endif
        // 使用默认主题
		ImGui::StyleColorsDark();
	}

    void ImFormManager::setAlpha(float alpha)
    {
        ImGui::GetStyle().Alpha = alpha;
    }

    void ImFormManager::setScale(float scale)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(scale);
        style.FontScaleDpi = scale;
    }

	void ImFormManager::setFont(const std::filesystem::path& file, float size)
	{
#ifndef USE_IMGUI_AS_RENDER_ENGINE
		if(!std::filesystem::exists(file))
		{
			LogError("font file({}) NOT found.", file.string());
			return;
		}

        auto& io = ImGui::GetIO();
		auto fonts = ImGui::GetIO().Fonts;
		auto font = fonts->AddFontFromFileTTF(file.string().c_str(), size, nullptr, fonts->GetGlyphRangesChineseFull());
        if(!font)
        {
			LogError("load font {} failed. use default.", file.string());
            io.Fonts->AddFontDefault();
        }
#endif
	}

	void ImFormManager::setStyle(ImGuiTheme style)
	{
		switch (style)
		{
		case ImGuiTheme::Classic: ImGui::StyleColorsClassic(); break;
		case ImGuiTheme::Dark: ImGui::StyleColorsDark(); break;
		case ImGuiTheme::Light: ImGui::StyleColorsLight(); break;
		}
	}

	void ImFormManager::quit()
	{
#ifndef USE_IMGUI_AS_RENDER_ENGINE
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();

		ImGui::DestroyContext();
#endif
	}

	void ImFormManager::processEvent(const SDL_Event& event)
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
	}

	bool ImFormManager::isAnyWindowHovered()
	{
		return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
			|| ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered();
	}

}
