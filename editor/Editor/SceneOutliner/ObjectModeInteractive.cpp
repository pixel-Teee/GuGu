#include <pch.h>

#include "ObjectModeInteractive.h"
#include "SceneOutliner.h"
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {

	ObjectModeInteractive::ObjectModeInteractive(const ObjectModeParams& params)
		: ObjectMode(params)
	{
		World::getWorld()->m_onLevelChanged.push_back(std::bind(&ObjectModeInteractive::onLevelChanged, this));
		//World::getWorld()->m_onObjectAdded.push_back(std::bind(&ObjectModeInteractive::onObjectAdded, this));
		//std::function<void(std::vector<std::shared_ptr<GameObject>>&)> func = std::bind(&ObjectModeInteractive::onObjectAdded, this, std::placeholders::_1);
		World::getWorld()->m_onObjectAdded.push_back(std::bind(&ObjectModeInteractive::onObjectAdded, this, std::placeholders::_1));
	}

	ObjectModeInteractive::~ObjectModeInteractive()
	{

	}

	void ObjectModeInteractive::onLevelChanged()
	{
		m_sceneOutliner->fullRefresh();
	}

	void ObjectModeInteractive::onObjectAdded(std::shared_ptr<GameObject>& inObject)
	{
		m_sceneOutliner->onLevelObjectAdded(inObject);
	}

}
