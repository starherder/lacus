#include "im_manager.h"

#include "SDL3/sdl.h"
#include "SDL3/SDL_events.h"

namespace imgui
{

	ImGuiManager::ImGuiManager()
	{
	}

	ImGuiManager::~ImGuiManager()
	{
		quit();
	}

	void ImGuiManager::pendingDestroyForm()
	{
		for (auto it = _forms.begin(); it != _forms.end(); it++ )
		{
			auto pForm = it->second;
			if (!pForm)
			{
				continue;
			}

			if (pForm->getPendingDestroy())
			{
				_forms.erase(it);
				return;
			}
		}
	}

	void ImGuiManager::closeForm(const std::string& name)
	{
		auto pForm = getImForm(name);
		if (pForm)
		{
			pForm->close();
		}
	}

	ImFormSharePtr ImGuiManager::getImForm(const std::string& name)
	{
		auto it = _forms.find(name);
		if (it == _forms.end())
		{
			return NULL;
		}

		auto pForm = it->second;
		return pForm;
	}

	void ImGuiManager::draw()
	{
		beforeDraw();

		pendingDestroyForm();

		for (auto itForm : _forms)
		{
			const std::string name = itForm.first;
			auto pForm = itForm.second;
			if (pForm)
			{
				pForm->drawForm();
			}
		}

		afterDraw();
	}

	void ImGuiManager::beforeDraw()
	{
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
	}

	void ImGuiManager::afterDraw()
	{
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);
	}

	void ImGuiManager::init(struct SDL_Window* window, struct SDL_Renderer* renderer)
	{
        _window = window;  
        _renderer = renderer;
		assert(_renderer);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

        // 初始化 ImGui 的 SDL3 和 SDL_Renderer3 后端
        ImGui_ImplSDL3_InitForSDLRenderer(_window, _renderer);
        ImGui_ImplSDLRenderer3_Init(_renderer);

        // 使用默认主题
		ImGui::StyleColorsDark();
	}

    void ImGuiManager::setAlpha(float alpha)
    {
        ImGui::GetStyle().Alpha = alpha;
    }

    void ImGuiManager::setScale(float scale)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(scale);        // 固定样式缩放比例。
        style.FontScaleDpi = scale;  
    }
    
	void ImGuiManager::setFontFile(const std::string& file, float size)
	{
        auto& io = ImGui::GetIO();
		auto fonts = ImGui::GetIO().Fonts;
		auto font = fonts->AddFontFromFileTTF(file.c_str(), size, NULL, fonts->GetGlyphRangesChineseFull());
        if(!font)
        {
            io.Fonts->AddFontDefault();
        }
	}

	void ImGuiManager::setStyle(ImGuiTheme style)
	{
		switch (style)
		{
		case ImGuiTheme::Classic: ImGui::StyleColorsClassic(); break;
		case ImGuiTheme::Dark: ImGui::StyleColorsDark(); break;
		case ImGuiTheme::Light: ImGui::StyleColorsLight(); break;
		}
	}

	void ImGuiManager::quit()
	{
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();

		ImGui::DestroyContext();
	}

	void ImGuiManager::processEvent(const SDL_Event& event)
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
	}

}