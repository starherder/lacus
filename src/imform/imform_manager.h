#pragma once

#include "imform.h"

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

	class ImFormManager final : 
                        public utility::ISingleton<ImFormManager>,
						public utility::sigslot::SlotHandler
	{
	public:
		typedef std::map<std::string, ImGuiFormSharePtr> ImGuiFormMap;

	public:
		ImFormManager();
		~ImFormManager();

		void init(struct SDL_Window* window, struct SDL_Renderer* renderer);

		void draw();

		template<class T, typename... Args>
		ImGuiFormSharePtr showForm(const std::string& name, Args... args);

		void closeForm(const std::string& name);

		ImGuiFormSharePtr getImGuiForm(const std::string& name);

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

		ImGuiFormMap _forms;
	};

	template<class T, typename... Args>
	ImGuiFormSharePtr ImFormManager::showForm(const std::string& name, Args... args)
	{
		auto pForm = getImGuiForm(name);
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
