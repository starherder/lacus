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
		auto views = _context.registry().view<CompNameId, CompSelection>();
		for (auto ent : views)
		{
			auto& nameComp = views.get<CompNameId>(ent);
			auto& selComp = views.get<CompSelection>(ent);

			selComp.selected = (nameComp.id == e.object);
		}
	}








}