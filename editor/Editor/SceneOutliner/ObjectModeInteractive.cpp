#include <pch.h>

#include "ObjectModeInteractive.h"
#include "SceneOutliner.h"
#include <Core/GamePlay/World.h>

namespace GuGu {

	ObjectModeInteractive::ObjectModeInteractive(const ObjectModeParams& params)
		: ObjectMode(params)
	{
		World::getWorld()->m_onLevelChanged.push_back(std::bind(&ObjectModeInteractive::onLevelChanged, this));
	}

	ObjectModeInteractive::~ObjectModeInteractive()
	{

	}

	void ObjectModeInteractive::onLevelChanged()
	{
		m_sceneOutliner->refresh();
	}

}
