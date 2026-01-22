#pragma once

#include "imform/imform.h"
#include "imform/imform_manager.h"
#include "engine/wrapper.h"

namespace game 
{
    using namespace engine;

    class GameContext;
    
    enum class DebugMode
    {
        Null,
        Select,
        PutObject,
    };


    class ImFormDebug : public imgui::ImForm
    {
    public:
        signals::Signal<bool> on_show_debug;
        signals::Signal<> on_reload_res;

    public:
        ImFormDebug();
        ~ImFormDebug();

        void init(GameContext* context);

	protected:

		void draw() override;

        void onMouseLeftClick(const Vec2& pos);
        void onMouseRightClick(const Vec2& pos);

        void moveSelectActor(const Vec2& pos);

        void roleExecSkill(entt::entity skill);

        void drawSkyWindow();
        
        void drawCameraWindow();

        void drawSelectEntityProps();

        void drawSkillWindow();

    public:
        entt::entity _selectEntity = entt::null;
        std::string _selectCfgId;

        GameContext* _context = nullptr;

        DebugMode _debugMode = DebugMode::Select;

        bool _showSkyWindow = false;
        bool _showCameraWindow = false;
        bool _showSkillWindow = false;

        Vec2 _windowPos, _windowSize;

        std::vector<const char*> _particleNames;
    };

}