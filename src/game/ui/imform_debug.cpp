#include "imform_debug.h"
#include "spdlog/spdlog.h"
#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"
#include "game/ecs/comp_fight.h"

#include "ui/gui_manager.h"
#include "game/ui/ui_logic_events.h"
#include "utility/translator.h"


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
    _context->eventDispatcher().onMouseRightClicked.connect(this, &ImFormDebug::onMouseRightClick);

    auto& particles = particle::ParticleManager::inst().GetAllParticleConfigs();
    for (auto& [name, file] : particles) {
        _particleNames.push_back(name.c_str());
    }
}

void ImFormDebug::draw()
{
    ImGui::Begin("debug");
    {
        auto ent_num = _context->registry().storage<entt::entity>().size();
        ImGui::Text("entt::entities: %lu", ent_num);

        ImGui::Separator();

        static bool show_mainform = true;
        if (ImGui::Checkbox("mainform##debug", &show_mainform))
        {
            ui::GuiManager::inst().emitCustomEvent(Event_ToggleMainForm, { show_mainform });
        }

        ImGui::SameLine();

        static bool show_debug = false;
        if(ImGui::Checkbox("show debug", &show_debug))
        {
            _context->scene().setDebugInfo(show_debug);
        }

        ImGui::Separator();

        if (ImGui::Button("sky")) 
        {
            _showSkyWindow = !_showSkyWindow;
        }

        if (ImGui::Button("reload resource"))
        {
            on_reload_res.emit();
        }

        ImGui::SameLine(0, 20);

        if (ImGui::Button("camera")) {
            _showCameraWindow = !_showCameraWindow;
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

        if (ImGui::RadioButton("put_obj##input", _debugMode == DebugMode::PutObject)) {
            _debugMode = DebugMode::PutObject;
        }
        ImGui::SameLine();

        static int select_index = 0;
        const auto& cfgs = ObjectFactory::inst().getAllObjectCfgIds();
        _selectCfgId = cfgs[select_index];

        if (ImGui::BeginCombo("##combo_cfgs", cfgs[select_index].c_str()))
        {
            for (int n = 0; n < cfgs.size(); n++)
            {
                bool is_selected = (select_index == n);
                if (ImGui::Selectable(cfgs[n].c_str(), is_selected))
                {
                    _selectCfgId = cfgs[n];
                    select_index = n;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        if (ImGui::Button("skill"))
        {
            _showSkillWindow = !_showSkillWindow;
        }

        //drawSelectEntityProps();

        ImVec2 winpos = ImGui::GetWindowPos();
        ImVec2 winsize = ImGui::GetWindowSize();
        _windowPos = {winpos.x, winpos.y};
        _windowSize = {winsize.x, winsize.y};
    }
    ImGui::End();

    if (_showSkyWindow)
    {
        drawSkyWindow();
    }

    if (_showCameraWindow)
    {
        drawCameraWindow();
    }

    if (_showSkillWindow)
    {
        drawSkillWindow();
    }
}

void ImFormDebug::onMouseLeftClick(const Vec2& pos)
{
    ImVec2 mousePos = ImGui::GetMousePos();

    if (mousePos.x > _windowPos.x && mousePos.x < _windowPos.x + _windowSize.x &&
        mousePos.y > _windowPos.y && mousePos.y < _windowPos.y + _windowSize.y)
    {
        return;
    }

    auto scenePos = _context->scene().camera().screenToWorld(pos);
    //SPDLOG_INFO("pos = ({},{}), scene_pos = ({}, {})", pos.x, pos.y, scenePos.x, scenePos.y);

    switch (_debugMode)
    {
    case DebugMode::Select:
    {
        _selectEntity = _context->scene().selectObjectAtPos(scenePos);
    }break;
    case DebugMode::PutObject:
    {
        _context->scene().createActor(_selectCfgId, scenePos);
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

void ImFormDebug::onMouseRightClick(const Vec2& pos)
{
    if(_debugMode == DebugMode::Select && _context->registry().valid(_selectEntity))
    {
        auto scenePos = _context->scene().camera().screenToWorld(pos);
        moveSelectActor(scenePos);
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

    if(_context->registry().try_get<CompMotion>(_selectEntity))
    {
        auto gridPos = _context->scene().getGridFromPos(pos);
        _context->dispatcher().trigger(EvtMoveToGrid{_selectEntity, gridPos, true});
    }
}

void ImFormDebug::drawSelectEntityProps()
{
    if(!_context->registry().valid(_selectEntity))
    {
        return;
    }

    std::map<std::string, std::string> props;

    auto pbase = _context->registry().try_get<CompBaseProp>(_selectEntity);
    if(pbase)
    {
        props.insert({ "str", std::to_string(pbase->str) });
        props.insert({ "cst", std::to_string(pbase->cst) });
        props.insert({ "dex", std::to_string(pbase->dex) });
        props.insert({ "met", std::to_string(pbase->met) });
    }
    auto pfight = _context->registry().try_get<CompFightProp>(_selectEntity);
    if (pfight)
    {
        props.insert({ "hp", std::to_string(pfight->hp) });
        props.insert({ "hpm", std::to_string(pfight->hpm) });
        props.insert({ "hpr", std::to_string(pfight->hpr) });
        props.insert({ "atk", std::to_string(pfight->atk) });
        props.insert({ "def", std::to_string(pfight->def) });
        props.insert({ "mvs", std::to_string(pfight->mvs) });
        props.insert({ "ats", std::to_string(pfight->ats) });
        props.insert({ "atd", std::to_string(pfight->atd) });
        props.insert({ "crt", std::to_string(pfight->crt) });
        props.insert({ "par", std::to_string(pfight->par) });
    }

    if(ImGui::BeginTable("props", 2))
    {
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("value");
        ImGui::TableHeadersRow();

        for(auto& [name, value] : props)
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text(name.c_str());


            ImGui::TableNextColumn();
            ImGui::Text(value.c_str());
        }
    }
    ImGui::EndTable();
}

void ImFormDebug::roleExecSkill(entt::entity skill)
{
    if (_selectEntity == entt::null ||
        _context->registry().valid(_selectEntity) == false)
    {
        return;
    }

    auto& trans = _context->registry().get<CompTransform>(_selectEntity);
    auto& rolePos = trans.position;

    auto& compName = _context->registry().get<CompNameId>(skill);
    auto& compSkill = _context->registry().get<CompSkillComm>(skill);

    // 不需要目标
    if (compSkill.type == SkillType::Other)
    {
        SPDLOG_INFO("skill ({}) need NO enmey.", compName.cfg_id);
        _context->dispatcher().trigger(EvtCastSkillToObject{ _selectEntity, entt::null, skill});
        return;
    }

    // 需要目标，寻找目标
    auto dis = compSkill.distance;
    auto& objects = _context->scene().getObjectsInCircle(rolePos, dis);
    for (auto& [dis, target] : objects) 
    {
        if (target == _selectEntity) { 
            continue; 
        }

        auto pdead = _context->registry().try_get<CompDead>(target);
        if (pdead) { 
            continue; 
        }

        auto pCompComm = _context->registry().try_get<CompComm>(target);
        if (pCompComm && pCompComm->type == ObjectType::Npc)  {
            _context->dispatcher().trigger(EvtCastSkillToObject{_selectEntity, target, skill});
            return;
        }
    }

    SPDLOG_INFO("skill ({}) find enemy faild.", compName.cfg_id);
}

void ImFormDebug::drawCameraWindow()
{
    ImGui::Begin("camera");

    ImGui::Text("shake");
    ImGui::Separator();

    static int duration = 0;
    ImGui::SliderInt("ms##camerashake", &duration, 100, 10000);

    static int freq = 0;
    ImGui::SliderInt("freq##camerashake", &freq, 1, 200);

    static int ampl = 0;
    ImGui::SliderInt("ampl##camerashake", &ampl, 10, 300);

    if (ImGui::Button("shake !")) 
    {
        _context->camera().shake(duration, freq, ampl);
    }

    ImGui::End();

}

void ImFormDebug::drawSkyWindow()
{
    ImGui::Begin("sky");

    static ImVec4 color;
    ImGui::ColorPicker4("color", (float*)& color);

    static int selectindex = 0;
    if (ImGui::BeginCombo("particle", _particleNames[selectindex]))
    {
        for (int i = 0; i < _particleNames.size(); i++)
        {
            bool select = (selectindex == i);
            if (ImGui::Selectable(_particleNames[i], &select))
            {
                selectindex = i;
            }
        }
        ImGui::EndCombo();
    }

    static int last = 2000;
    ImGui::SliderInt("last_ms", &last, 1000, 60000);

    static int fadein = 1000;
    ImGui::SliderInt("fadein_ms", &fadein, 1000, 10000);

    static int fadeout = 1000;
    ImGui::SliderInt("fadeout_ms", &fadeout, 1000, 10000);

    if (ImGui::Button("start"))
    {
        Color c = { color.x, color.y, color.z, color.w };
        std::string particle = _particleNames[selectindex];

        _context->objectFactory().createSkyEffect(SkyEffect::Dark, c, last, fadein, fadeout);
    }

    ImGui::End();
}

void ImFormDebug::drawSkillWindow()
{
    ImGui::Begin("skills");

    if (_selectEntity == entt::null ||
        _context->registry().valid(_selectEntity) == false)
    {
        ImGui::End();
        return;
    }

    auto& skills = _context->registry().get<CompSkills>(_selectEntity);
    for (auto& skill_id : skills.skills)
    {
        auto& compName = _context->registry().get<CompNameId>(skill_id);
        auto& compSkill = _context->registry().get<CompSkillComm>(skill_id);

        auto skill_name = std::format("{}##_btn", Trans(compName.name));
        if (ImGui::Button(skill_name.c_str()))
        {
            roleExecSkill(skill_id);
        }
    }

    ImGui::End();
}

}