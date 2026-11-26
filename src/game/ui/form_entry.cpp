#include "form_entry.h"
#include "form_config.h"
#include "form_cards.h"

#include "game/ui/ui_logic_events.h"
#include "game/ui/form_utils.h"
#include "game/scene/object_manager.h"
#include "game/scene/game_data.h"
#include "game/scene/scene_config.h"
#include "game/logic/game_logic.h"

#include <algorithm>

namespace game 
{

FormEntry::FormEntry(const std::string& name, GameContext& context) : FormLogicBase(name, context)
{
	load(_context.resPath() / "ui/form_entry.xml");
	
	showHandCards();

	if (auto formCards = ui::GuiManager::inst().getForm<FormCards>("form_cards"))
	{
		formCards->onHandCardsConfirmed.connect(this, &FormEntry::showHandCards);
	}

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
}

FormEntry::~FormEntry()
{
}

void FormEntry::selectScene(const std::string& scene)
{
	_sceneFile = scene;

	showSceneInfo();
}

void FormEntry::onUpdate(float delta)
{
}

void FormEntry::onStart(Button* btn)
{
	_context.gameLogic().switchScene(_sceneFile);

	_context.gameLogic().setGameMode(SceneGameMode::GameMode_AutoChess);
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
		formCards = ui::GuiManager::inst().showForm<FormCards>("form_cards", _context);
		formCards->onHandCardsConfirmed.connect(this, &FormEntry::showHandCards);
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

void FormEntry::showHandCards()
{
	auto cardGroup = getWidget<CardGroup>("card_group");
	if (!cardGroup)
	{
		LogInfo("FormEntry::showHandCards card_group NOT found.");
		return;
	}

	cardGroup->clear();

	for (const auto& [cfg, handCard] : _context.gameData().getHandCards())
	{
		auto& props = _context.objectManager().getObjectCfgProperties(cfg);
		auto card = cardGroup->addCard(props);
		FormUtils::addRankMarker(card, handCard.rank);
	}
}

void FormEntry::showSceneInfo()
{
	auto mapConfig = _context.sceneConfig().getMapConfig(_sceneFile);
	if (!mapConfig)
	{
		LogWarn("FormEntry::showSceneInfo scene config ({}) not found.", _sceneFile);
		return;
	}

	if (auto icon = getWidget<Label>("lbl_scene_icon"))
	{
		if (!mapConfig->icon.empty())
		{
			auto tile = ui::GuiManager::inst().getCfgTexTile(mapConfig->icon);
			if (tile)
			{
				icon->setTexTile(tile);

				const Vec2 maxIconSize = { 320.0f, 220.0f };
				auto tileSize = tile->rect().size();
				auto scale = std::min(maxIconSize.x / tileSize.x, maxIconSize.y / tileSize.y);
				icon->setSize(tileSize * scale);
			}
		}
	}

	if (auto name = getWidget<Label>("lbl_scene_name"))
	{
		name->setText(mapConfig->name);
	}

	if (auto level = getWidget<Label>("lbl_scene_level"))
	{
		level->setText("Lv." + std::to_string(mapConfig->level));
	}

	if (auto desc = getWidget<TextBox>("txt_scene_desc"))
	{
		desc->setText(mapConfig->desc);
	}
}
}
