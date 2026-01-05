#include "imform_debug.h"
#include "spdlog/spdlog.h"
#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"

namespace game 
{

ImFormDebug::ImFormDebug()
{
}

ImFormDebug::~ImFormDebug()
{
}

void ImFormDebug::init(GameContext* context)
{
    _context = context;

    _context->eventDispatcher().onMouseLeftClicked.connect(this, &ImFormDebug::onMouseLeftClick);
}

void ImFormDebug::draw()
{
    ImGui::Begin("debug");
    {
        static bool show_debug = false;
        if(ImGui::Checkbox("show debug", &show_debug))
        {
            _context->currentScene().setDebugInfo(show_debug);
        }
    
        ImGui::Separator();

        ImGui::Text("input");

        ImGui::SameLine();
        if (ImGui::RadioButton("null##input", _debugMode == DebugMode::Null)) {
            _debugMode = DebugMode::Null;
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("select##input", _debugMode == DebugMode::Select)) {
            _debugMode = DebugMode::Select;
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("moveto##input", _debugMode == DebugMode::MoveToGrid)) {
            _debugMode = DebugMode::MoveToGrid;
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("put_obj##input", _debugMode == DebugMode::PutObject)) {
            _debugMode = DebugMode::PutObject;
        }

        ImGui::SameLine();
        if (ImGui::RadioButton("put_role##input", _debugMode == DebugMode::PutRole)) {
            _debugMode = DebugMode::PutRole;
        }

        ImVec2 winpos = ImGui::GetWindowPos();
        ImVec2 winsize = ImGui::GetWindowSize();
        _windowPos = {winpos.x, winpos.y};
        _windowSize = {winsize.x, winsize.y};
    }
    ImGui::End();
}

void ImFormDebug::onMouseLeftClick(const Vec2& pos)
{
    ImVec2 mousePos = ImGui::GetMousePos();

    if (mousePos.x > _windowPos.x && mousePos.x < _windowPos.x + _windowSize.x &&
        mousePos.y > _windowPos.y && mousePos.y < _windowPos.y + _windowSize.y)
    {
        return;
    }

    auto scenePos = _context->currentScene().camera().screenToWorld(pos);
    spdlog::info("pos = ({},{}), scene_pos = ({}, {})", pos.x, pos.y, scenePos.x, scenePos.y);

    switch (_debugMode)
    {
    case DebugMode::Select:
    {
        _selectEntity = _context->currentScene().selectObjectAtPos(scenePos);
    }break;
    case DebugMode::MoveToGrid:
    {
        moveSelectActor(scenePos);
    }break;
    case DebugMode::PutObject:
    {
        std::string item_cfg = "item_0";
        _context->currentScene().createObject(item_cfg, scenePos);
    }break;
    case DebugMode::PutRole:
    {
        std::string npc_cfg = "npc_0";
        _context->currentScene().createObject(npc_cfg, scenePos);
    }break;
    case DebugMode::Null:
    {
        _selectEntity = entt::null;
    }break;
    default:
    {
    }break;
    }
}

void ImFormDebug::moveSelectActor(const Vec2& pos)
{
    if (_selectEntity == entt::null || 
        _context->registry().valid(_selectEntity) == false)
    {
        return;
    }

    auto bevComp = _context->registry().try_get<CompBehavior>(_selectEntity);
    if (bevComp && bevComp->bevtree)
    {
        bevComp->bevtree->stop();
    }

    auto gridPos = _context->currentScene().getGridFromPos(pos);
    _context->dispatcher().trigger(MoveToGrid{_selectEntity, gridPos, true});
}

}