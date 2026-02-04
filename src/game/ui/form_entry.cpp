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
	
	showCardGroup();

	auto btnEnter = getWidget<ui::Button>("btn_enter");
	if (btnEnter)
	{
		btnEnter->on_click.connect(this, &FormEntry::onStart);
	}

	auto btnCards = getWidget<ui::Button>("btn_cards");
	if (btnCards)
	{
		btnCards->on_click.connect(this, &FormEntry::onShowCardDeck);
	}

	auto btnLeave = getWidget<ui::Button>("btn_leave");
	if (btnLeave)
	{
		btnLeave->on_click.connect([this](ui::Button* btn) { close(); });
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
	ui::GuiManager::inst().emitCustomEvent(Event_SelectScene, 
		{ _sceneFile, (int)SceneGameMode::GameMode_AutoChess });

	_context.dataCenter().clearHandCard();

	auto& cardGroup = _context.dataCenter().getCardGroup();
	for (auto& card : cardGroup) 
	{
		_context.dataCenter().addHandCard(card);
	}
}

void FormEntry::onShowCardDeck(Button* btn)
{
	auto formCards = ui::GuiManager::inst().getForm<FormCards>("form_cards");
	if (formCards)
	{
		ui::GuiManager::inst().closeForm("form_cards");
	}
	else
	{
		ui::GuiManager::inst().createForm<FormCards>("form_cards", _context);
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
		LogInfo("FormEntry::onDropCard dragging data error.");
		return;
	}

	auto card = dynamic_cast<CardWidget*>(ptr->widget.get());
	auto srcCardGroup = dynamic_cast<CardGroup*>(ptr->src_group);
	if (!card || !srcCardGroup)
	{
		LogError("FormEntry::onDropCard: drop item is NOT card.");
		return;
	}

	auto cfgid = card->getCfgid();
	auto found = _context.objectFactory().findObjectCfg(cfgid);
	if (!found)
	{
		LogError("FormEntry::onDropCard: drop item cfg({}) NOT found.", cfgid);
		return;
	}

	auto handCardGroup = getWidget<CardGroup>("card_group");

	// remove from hand card group
	if (ptr->src_group == handCardGroup && ptr->dst_group != handCardGroup)
	{
		handCardGroup->removeCard(card);
		_context.dataCenter().removeFromCardGroup(card->getCfgid());
		return;
	}

	// add to hand card group
	if (ptr->dst_group == handCardGroup)
	{
		auto& props = _context.objectFactory().getObjectCfgProperties(cfgid);
		handCardGroup->addCard(props);

		_context.dataCenter().addToCardGroup(cfgid);

		int index = card->getData<int>("index");
		srcCardGroup->addWidget(ptr->widget, index);
	}
}

void FormEntry::showCardGroup()
{
	auto cardGroup = getWidget<CardGroup>("card_group");
	if (!cardGroup)
	{
		LogInfo("FormEntry::loadCars card_group NOT found.");
		return;
	}

	auto& roleCfgs = _context.dataCenter().getCardGroup();
	for (auto& cfg : roleCfgs)
	{
		auto& props = _context.objectFactory().getObjectCfgProperties(cfg);
		cardGroup->addCard(props);
	}

	_context.dataCenter().clearHandCard();
}
}