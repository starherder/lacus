#pragma once

#include "game/ui/form_logic_base.h"

namespace game
{
	class FormBag : public FormLogicBase
	{
	public:
		FormBag(const std::string& name) = delete;
		FormBag(const std::string& name, GameContext& context);
		~FormBag();

		void refreshBag();

	private:
		void onClearBag(ui::Button* btn);
		void addBagItemWidget(const std::string& cfgid, int amount, int index);

	private:
		ui::GridGroup* _gridBag = nullptr;
	};
}
