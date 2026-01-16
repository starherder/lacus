#pragma once

#include "ui/ui.h"
#include "game/scene/game_context.h"


namespace game
{
	using namespace ui;

	class FormLogicBase : public Form
	{
	public:
		FormLogicBase(const std::string& name) = delete;
		FormLogicBase(const std::string& name, GameContext& context)
			: Form(name), _context(context)
		{}

	protected:
		GameContext& _context;

	};
}