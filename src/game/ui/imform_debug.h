#pragma once

#include "imform/imform.h"
#include "imform/imform_manager.h"
#include "engine/wrapper.h"
#include "game/ecs/comm_comp.h"

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

    struct GridInfo
    {
        Vec2i grid = {0,0};
        std::set<entt::entity> objs;
    };

    class ImFormDebug : public imgui::ImForm
    {
    public:
        signals::Signal<bool> on_show_debug;
        signals::Signal<> on_reload_res;
        signals::Signal<> on_reload_script;
        
    public:
        ImFormDebug();
        ~ImFormDebug();

        void init(GameContext* context);

        void showGridInfo(const Vec2i& grid, const std::set<entt::entity>& objs);
        
        void showTurnInfo(int turnType);

	protected:

		void draw() override;

        void roleExecSkill(entt::entity skill);

        void drawSkyWindow();
        
        void drawCameraWindow();

        void drawStoryWindow();

        void drawSelectEntityProps();

        void drawSkillWindow();

        void drawSceneInfo();
        
        void onMouseLeftClick(const Vec2& pos);

    public:
        entt::entity _selectEntity = entt::null;
        std::string _selectCfgId;
        CampSide _campSide = CampSide::Officer;

        GameContext* _context = nullptr;

        DebugMode _debugMode = DebugMode::Select;

        bool _showSkyWindow = false;
        bool _showCameraWindow = false;
        bool _showStoryWindow = false;

        Vec2 _windowPos, _windowSize;

        std::vector<const char*> _particleNames;
        std::vector<std::string> _emotionNames;

        GridInfo _curGridInfo;
        int _turnInfo;
    };

}