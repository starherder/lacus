#include "form_chess_tip.h"
#include "game/ecs/comm_comp.h"
#include "game/ecs/comp_fight.h"
#include "utility/translator.h"

namespace game
{

    FormChessTip::FormChessTip(const std::string& name, GameContext& context) : FormLogicBase(name, context)
    {
        load(_context.resPath() / "ui/form_chess_tip.xml");

        _lblName = getWidget<Label>("lbl_name");
        _lblLevel = getWidget<Label>("lbl_lv");
        _listProps = getWidget<ListBox>("list_props");

        assert(_lblName && _lblLevel && _listProps);
    }

    FormChessTip::~FormChessTip()
    {
    }

    void FormChessTip::showChessTip(entt::entity obj)
    {
        auto pNameId = _context.registry().try_get<CompNameId>(obj);
        if (pNameId)
        {
            _lblName->setText(pNameId->name);
        }

        auto pBaseProps = _context.registry().try_get<CompBaseProp>(obj);
        if (pBaseProps) 
        {
            _lblLevel->setText(std::format("{}:{}", Trans("LV"), pBaseProps->lv));
        }

        auto pFightProps = _context.registry().try_get<CompFightProp>(obj);
        if (pFightProps)
        {
            std::vector<std::pair<std::string, float>> propsMap = {
                {"HPM", pFightProps->hpm}, 
                {"HPR", pFightProps->hpr},
                {"ATK", pFightProps->atk},
                {"DEF", pFightProps->def},
                {"MVS", pFightProps->mvs},
                {"ATS", pFightProps->ats},
                {"ATD", pFightProps->atd},
                {"CRT", pFightProps->crt},
                {"PAR", pFightProps->par},
            };

            _listProps->clear();

            for (auto& [name, val] : propsMap)
            {
                auto str = std::format("{} : {:.1f}", Trans(name), val);
                _listProps->addItem(str);
            }
        }
    }

    void FormChessTip::onUpdate(float delta)
    {
        auto mousePos = _context.eventDispatcher().mousePos();
        setPos(mousePos + Vec2{10, 10});
    }

    void FormChessTip::onSizeChanged()
    {
    }
}