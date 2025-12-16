#include "imform_manager.h"

#include "SDL3/sdl.h"
#include "SDL3/SDL_events.h"
#include "spdlog/spdlog.h"

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

	void ImFormManager::closeForm(const std::string& name)
	{
		auto pForm = getImGuiForm<ImForm>(name);
		if (pForm)
		{
			pForm->close();
		}
	}

	void ImFormManager::draw()
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

	void ImFormManager::beforeDraw()
	{
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
	}

	void ImFormManager::afterDraw()
	{
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _renderer);
	}

	void ImFormManager::init(struct SDL_Window* window, struct SDL_Renderer* renderer)
	{
		spdlog::info("ImFormManager::init");;

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

    void ImFormManager::setAlpha(float alpha)
    {
        ImGui::GetStyle().Alpha = alpha;
    }

    void ImFormManager::setScale(float scale)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(scale);        // 固定样式缩放比例。
        style.FontScaleDpi = scale;  
    }
    
	void ImFormManager::setFont(const std::filesystem::path& file, float size)
	{
		if(!std::filesystem::exists(file))
		{
			spdlog::error("font file({}) NOT found.", file.string());
			return;
		}

        auto& io = ImGui::GetIO();
		auto fonts = ImGui::GetIO().Fonts;
		auto font = fonts->AddFontFromFileTTF(file.string().c_str(), size, nullptr, fonts->GetGlyphRangesChineseFull());
        if(!font)
        {
			spdlog::error("load font {} failed. use default.", file.string());
            io.Fonts->AddFontDefault();
        }
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
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();

		ImGui::DestroyContext();
	}

	void ImFormManager::processEvent(const SDL_Event& event)
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
	}

}
