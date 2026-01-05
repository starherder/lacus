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
        MoveToGrid,
        PutObject,
        PutRole,
    };


    class ImFormDebug : public imgui::ImForm
    {
    public:
        signals::Signal<bool> on_show_debug;

    public:
        ImFormDebug();
        ~ImFormDebug();

        void init(GameContext* context);

	protected:

		void draw() override;

        void onMouseLeftClick(const Vec2& pos);

        void moveSelectActor(const Vec2& pos);

    public:
        entt::entity _selectEntity = entt::null;

        GameContext* _context = nullptr;

        DebugMode _debugMode = DebugMode::Null;

        Vec2 _windowPos, _windowSize;
    };

}