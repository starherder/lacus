#include "form_bag.h"

#include "game/scene/game_data.h"
#include "game/scene/object_manager.h"

namespace game
{
	FormBag::FormBag(const std::string& name, GameContext& context) : FormLogicBase(name, context)
	{
		load(_context.resPath() / "ui/form_bag.xml");

		auto btnClose = getWidget<ui::Button>("btn_close");
		if (btnClose)
		{
			btnClose->on_click.connect([this](ui::Button* btn) { close(); });
		}

		auto btnClear = getWidget<ui::Button>("btn_clear");
		if (btnClear)
		{
			btnClear->on_click.connect(this, &FormBag::onClearBag);
		}

		_gridBag = getWidget<ui::GridGroup>("grid_bag");
		assert(_gridBag);

		refreshBag();
	}

	FormBag::~FormBag()
	{
	}

	void FormBag::refreshBag()
	{
		if (!_gridBag)
		{
			return;
		}

		_gridBag->clear();

		const auto& bagItems = _context.gameData().getBagItems();
		if (bagItems.empty())
		{
			return;
		}

		int index = 0;
		for (const auto& [cfgid, item] : bagItems)
		{
			addBagItemWidget(cfgid, item.amount, index++);
		}
	}

	void FormBag::onClearBag(ui::Button* btn)
	{
		_context.gameData().clearBagItems();
		refreshBag();
	}

	void FormBag::addBagItemWidget(const std::string& cfgid, int amount, int index)
	{
		if (!_gridBag)
		{
			return;
		}

		std::string itemName = cfgid;
		std::string itemIcon;
		if (amount > 0 && _context.objectManager().findObjectCfg(cfgid))
		{
			const auto& props = _context.objectManager().getObjectCfgProperties(cfgid);
			if (auto name = props.get<std::string>("name"))
			{
				itemName = name.value();
			}
			if (auto icon = props.get<std::string>("icon"))
			{
				itemIcon = icon.value();
			}
		}

		auto itemGroup = std::make_shared<ui::Group>("bag_item_" + std::to_string(index), _gridBag);
		itemGroup->setBgColor(Color{ 246, 250, 242, 238 });
		itemGroup->setBorderColor(Color{ 47, 107, 63, 255 });
		itemGroup->setBorderSize(2.0f);
		itemGroup->setBorderRound(6.0f);
		itemGroup->setAcceptEvent(false);
		itemGroup->setSize({ 150.0f, 72.0f });

		auto icon = itemGroup->createChild<ui::Label>("icon");
		icon->setText("");
		icon->setPos({ 10.0f, 12.0f });
		icon->setSize({ 48.0f, 48.0f });
		icon->setBgColor(Color::White);
		icon->setBorderColor(Color{ 47, 107, 63, 255 });
		icon->setBorderSize(1.0f);
		icon->setBorderRound(4.0f);
		icon->setAcceptEvent(false);
		if (!itemIcon.empty())
		{
			icon->setTexTile(itemIcon);
		}

		auto name = itemGroup->createChild<ui::Label>("name");
		name->setText(itemName);
		name->setFont("fonts/msyh.ttf", 16);
		name->setTextColor(Color::Dark);
		name->setTextAlign(ui::Align::Left);
		name->setTextPadding({ 2.0f, 0.0f });
		name->setPos({ 64.0f, 14.0f });
		name->setSize({ 76.0f, 24.0f });
		name->setBgColor(Color::Invalid);
		name->setBorderColor(Color::Invalid);
		name->setAcceptEvent(false);

		auto count = itemGroup->createChild<ui::Label>("count");
		count->setText("x" + std::to_string(amount));
		count->setFont("fonts/msyh.ttf", 15);
		count->setTextColor(Color{ 82, 68, 47, 255 });
		count->setTextAlign(ui::Align::Left);
		count->setTextPadding({ 2.0f, 0.0f });
		count->setPos({ 64.0f, 38.0f });
		count->setSize({ 76.0f, 22.0f });
		count->setBgColor(Color::Invalid);
		count->setBorderColor(Color::Invalid);
		count->setAcceptEvent(false);

		_gridBag->addWidget(itemGroup);
	}
}
