#pragma once

#include "im_form.h"

#include <filesystem>
#include <memory>

#include "utility/signalslot.h"
#include "utility/i_singleton.h"

namespace imgui
{
	enum class ImGuiTheme
	{
		Classic,
		Dark,
		Light,
	};

	class ImGuiManager final : 
                        public utility::ISingleton<ImGuiManager>,
						public utility::sigslot::SlotHandler
	{
	public:
		typedef std::map<std::string, ImFormSharePtr> ImFormMap;

	public:
		ImGuiManager();
		~ImGuiManager();

		void init(struct SDL_Window* window, struct SDL_Renderer* renderer);

		void draw();

		template<class T, typename... Args>
		ImFormSharePtr showForm(const std::string& name, Args... args);

		void closeForm(const std::string& name);

		ImFormSharePtr getImForm(const std::string& name);

		void setFont(const std::filesystem::path& file, float size);

		void setStyle(ImGuiTheme style);

        void setAlpha(float alpha);

        void setScale(float scale);

		void processEvent(const union SDL_Event& event);

	private:

		void quit();

		void beforeDraw();

		void afterDraw();

		void pendingDestroyForm();

	private:
        struct SDL_Window* _window = nullptr;
		struct SDL_Renderer* _renderer = nullptr;

		ImFormMap _forms;
	};

	template<class T, typename... Args>
	ImFormSharePtr ImGuiManager::showForm(const std::string& name, Args... args)
	{
		auto pForm = getImForm(name);
		if (pForm)
		{
			pForm->show(true);
			return pForm;
		}

		pForm = std::make_shared<T>(args...);
		assert(pForm);

		pForm->setName(name);

		_forms[name] = pForm;
		pForm->show(true);
		return pForm;
	}
}