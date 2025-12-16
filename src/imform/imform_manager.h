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

		template<typename T>
		T* getImGuiForm(const std::string& name);

		template<class T, typename... Args>
		T* showForm(const std::string& name, Args... args);

		void closeForm(const std::string& name);

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

	template<typename T>
	T* ImFormManager::getImGuiForm(const std::string& name)
	{
		auto it = _forms.find(name);
		if (it == _forms.end())
		{
			return nullptr;
		}

		auto pForm = it->second;
		return (T*)pForm.get();
	}

	template<class T, typename... Args>
	T* ImFormManager::showForm(const std::string& name, Args... args)
	{
		auto form = getImGuiForm<T>(name);
		if (form)
		{
			form->show(true);
			return form;
		}

		auto pForm = std::make_shared<T>(args...);
		assert(pForm);

		pForm->setName(name);

		_forms[name] = pForm;
		pForm->show(true);
		return (T*)pForm.get();
	}
}
