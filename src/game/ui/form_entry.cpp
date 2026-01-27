#include "form_entry.h"
#include "form_config.h"
#include "form_cards.h"

#include "game/ui/ui_logic_events.h"
#include "game/scene/object_factory.h"
#include "game/scene/game_data.h"

namespace game 
{

FormEntry::FormEntry(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	load(_context.resPath() / "ui/form_entry.xml");
	
	loadCars();

	auto btnEnter = getWidget<ui::Button>("btn_enter");
	if (btnEnter)
	{
		btnEnter->on_click.connect(this, &FormEntry::onStart);
	}

	auto btnCards = getWidget<ui::Button>("btn_cards");
	if (btnCards)
	{
		btnCards->on_click.connect(this, &FormEntry::onCards);
	}

	auto btnLeave = getWidget<ui::Button>("btn_leave");
	if (btnLeave)
	{
		btnLeave->on_click.connect([this](ui::Button* btn) { close(); });
	}

	auto formCards = ui::GuiManager::inst().createForm<FormCards>("form_cards", _context);
	if (formCards)
	{
		formCards->setVisible(false);
	}

	ui::GuiManager::inst().on_drop.connect(this, &FormEntry::onDropCard);
}

FormEntry::~FormEntry()
{
}

void FormEntry::selectScene(const std::string& scene)
{
	_sceneFile = scene;

	//showDescript();
}

void FormEntry::onUpdate(float delta)
{
}

void FormEntry::onStart(Button* btn)
{
	ui::GuiManager::inst().emitCustomEvent(Event_SelectScene, { _sceneFile });
}

void FormEntry::onCards(Button* btn)
{
	auto formCards = ui::GuiManager::inst().getForm<FormCards>("form_cards");
	if (formCards)
	{
		formCards->setVisible(!formCards->visible());
	}

	resize();
}

void FormEntry::onSizeChanged()
{
}

void FormEntry::onWindowResized(const Vec2& sz)
{
	FormLogicBase::onWindowResized(sz);

	resize();
}

void FormEntry::resize()
{
	auto wndSz = ui::GuiManager::inst().windowSize();

	Vec2 formpos = {0,0};
	auto vlayMain = getWidget<Widget>("vlay_main");
	if (vlayMain) {
		formpos = { vlayMain->size().x, 0 };
	}

	Vec2 formsz = {size().x - formpos.x, size().y};

	auto formCards = ui::GuiManager::inst().getForm<FormCards>("form_cards");
	if (formCards)
	{
		formCards->setMaximize(false);
		formCards->setPos(formpos);
		formCards->setSize(formsz);
		formCards->moveToTop();
	}
}

void FormEntry::onDropCard(ui::GuiManager::DraggingPtr ptr)
{
	if (!ptr || !ptr->dst_group)
	{
		SPDLOG_INFO("FormEntry::onDropCard dragging data error.");
		return;
	}

	auto dstCardGroup = getWidget<CardGroup>("card_group");
	if (ptr->dst_group != dstCardGroup)
	{
		SPDLOG_INFO("FormEntry::onDropCard not drop to card_group.");
		return;
	}

	auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
	auto srcCardGroup = dynamic_cast<CardGroup*>(ptr->src_group);
	if (!card || !srcCardGroup)
	{
		SPDLOG_ERROR("FormEntry::onDropCard: drop item is NOT card.");
		return;
	}

	auto cfgid = card->getCfgid();
	auto found = _context.objectFactory().findObjectCfg(cfgid);
	if (!found)
	{
		SPDLOG_ERROR("FormEntry::onDropCard: drop item cfg({}) NOT found.", cfgid);
		return;
	}

	auto& props = _context.objectFactory().getObjectCfgProperties(cfgid);
	dstCardGroup->addCard(props);

	_context.dataCenter().addHandCards(cfgid);

	//int index = card->getData<int>("index");
	//srcCardGroup->addWidget(ptr->widget, index);
}

void FormEntry::loadCars()
{
	auto cardGroup = getWidget<CardGroup>("card_group");
	if (!cardGroup)
	{
		SPDLOG_INFO("FormEntry::loadCars card_group NOT found.");
		return;
	}

	auto& roleCfgs = _context.dataCenter().getHandCards();
	for (auto& cfg : roleCfgs)
	{
		auto& props = _context.objectFactory().getObjectCfgProperties(cfg);
		cardGroup->addCard(props);
	}
}
}