#pragma once

#include "engine/application.h"
#include "imform/imform.h"
#include "imform/imform_manager.h"
#include "utility/i_singleton.h"

namespace samples
{
    // HUB
	class ImGuiFormHUD : public imgui::ImForm
	{
	public:
		ImGuiFormHUD() = default;
		ImGuiFormHUD(engine::Application* app) : _application(app) {}
		~ImGuiFormHUD() = default;

	protected:
		void draw() override;

	private:
		engine::Application* _application = nullptr;
	};

    // 主窗口
	class ImFormMainFrame : public imgui::ImForm
	{
	protected:
		void draw() override;

        void drawMainMenu();
        void drawMenuPlugins();
	};
    
    // 主插件
    class SamplePluginMain final : public engine::Plugin 
    {
    public:
        SamplePluginMain(engine::Application& app);
        SamplePluginMain() = delete;
        ~SamplePluginMain() = default;
        
        const char* name() override { return "sample_main_plugin"; }
        void onInit() override;
        void onClose() override;
        void onUpdate() override;
        void onDraw() override;
        void onDrawUI() override;

    private:
        engine::Application& _app;
    };

    // 插件管理器
    class SamplePluginManager : public utility::ISingleton<SamplePluginManager>
    {
        using PluginUniquePtr = std::unique_ptr<engine::Plugin>;

    public:
        void init(engine::Application& app);

        template<typename T, typename... Args>
        T* addRootPlugin(Args&&... args);

        template<typename T, typename... Args>
        T* addNormalPlugin(Args&&... args);

        bool removePlugin(const std::string& name);

        void setPluginEnable(const std::string& name, bool enabled);

    private:
        bool addRootPlugin(PluginUniquePtr plugin);

        engine::Application* _app = nullptr;
    };

    
    template<typename T, typename... Args>
    T* SamplePluginManager::addRootPlugin(Args&&... args)
    {
        return _app->addPlugin<T>(engine::PluginPriority::Bottom, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T* SamplePluginManager::addNormalPlugin(Args&&... args)
    {
        return _app->addPlugin<T>(engine::PluginPriority::Default, std::forward<Args>(args)...);
    }
    
}