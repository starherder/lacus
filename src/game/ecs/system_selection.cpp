#include "system_selection.h"



namespace game 
{

	SelectionSystem::SelectionSystem(GameContext& context) : EcsSystem(context)
	{
		context.dispatcher().sink<ObjectSelection>().connect<&SelectionSystem::onObjectSelection>(this);
	}

	SelectionSystem::~SelectionSystem()
	{

	}

	void SelectionSystem::update(float delta)
	{
	}

	void SelectionSystem::onObjectSelection(const ObjectSelection& e)
	{
		auto views = _context.registry().view<CompSelection>();
		for (auto ent : views)
		{
			_context.registry().remove<CompSelection>(ent);
		}

		if(_context.registry().valid(e.object))
		{
			_context.registry().emplace<CompSelection>(e.object, CompSelection{});
		}
	}








}