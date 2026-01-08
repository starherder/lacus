#include "imform_debug.h"
#include "spdlog/spdlog.h"
#include "game/scene/game_scene.h"
#include "game/scene/game_context.h"
#include "game/ecs/comp_fight.h"




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


    auto& particles = particle::ParticleManager::inst().GetAllParticleConfigs();
    for (auto& [name, file] : particles) {
        _particleNames.push_back(name.c_str());
    }
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

        if(ImGui::Button("reload particle"))
        {
            particle::ParticleManager::inst().Reload();
        }

        if (ImGui::Button("sky")) {
            _showSkyWindow = !_showSkyWindow;
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
        if (ImGui::Button("exec_skill"))
        {
            roleExecSkill();
        }

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
    //spdlog::info("pos = ({},{}), scene_pos = ({}, {})", pos.x, pos.y, scenePos.x, scenePos.y);

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
        _context->currentScene().createObject(_selectCfgId, scenePos);
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

void ImFormDebug::roleExecSkill()
{
    if (_selectEntity == entt::null ||
        _context->registry().valid(_selectEntity) == false)
    {
        return;
    }

    auto& trans = _context->registry().get<CompTransform>(_selectEntity);
    auto& rolePos = trans.position;

    auto& skills = _context->registry().get<CompSkills>(_selectEntity);
    for (auto& skent : skills.skills)
    {
        auto& compName = _context->registry().get<CompNameId>(skent);
        auto& compSkill = _context->registry().get<CompSkillComm>(skent);

        if (compSkill.type == SkillType::Combat || compSkill.type == SkillType::Projectile)
        {
            // 需要目标，寻找目标
            auto dis_far = compSkill.distance.y;

            auto& objects = _context->currentScene().getObjectsInCircle(rolePos, dis_far);
            for (auto& [dis, target] : objects) {
                if (target == _selectEntity) continue;

                auto& compComm = _context->registry().get<CompComm>(target);
                if (compComm.type == ObjectType::Npc) {

                    _context->dispatcher().trigger(RoleExecSkillToObject{_selectEntity, target, skent});
                    return;
                }
            }

            spdlog::info("skill ({}) find enemy faild.", compName.cfg_id);
        }
        else
        {
            spdlog::info("skill ({}) need NO enmey.", compName.cfg_id);

            _context->dispatcher().trigger(RoleExecSkillToObject{ _selectEntity, entt::null, skent });
        }

    }
}

void ImFormDebug::drawSkyWindow()
{
    ImGui::Begin("windows");

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



}