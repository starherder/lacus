#pragma once

#include "bevtree/bevtree.h"

namespace game 
{
	using namespace bevtree;

	class BevNode_FindIdlePos : public BevNode
	{
	public:
		bool load(const XmlNode* node) override;

		Status update() override;
		
		void initialize() override;

		void terminate(Status s) override;
	};


}