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
		World::getWorld()->m_onObjectRemoved.push_back(std::bind(&ObjectModeInteractive::onObjectRemoved, this, std::placeholders::_1));

		World::getWorld()->m_onObjectAttached.push_back(std::bind(&ObjectModeInteractive::onObjectAttached, this, std::placeholders::_1, std::placeholders::_2));
		World::getWorld()->m_onObjectDetched.push_back(std::bind(&ObjectModeInteractive::onObjectDetached, this, std::placeholders::_1, std::placeholders::_2));
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

	void ObjectModeInteractive::onObjectRemoved(std::shared_ptr<GameObject>& inObject)
	{
		m_sceneOutliner->onLevelObjectRemoved(inObject);
	}

	void ObjectModeInteractive::onObjectAttached(std::shared_ptr<GameObject>& inObject, std::shared_ptr<GameObject>& inParent)
	{
		m_sceneOutliner->onLevelObjectAttached(inObject, inParent);
	}

	void ObjectModeInteractive::onObjectDetached(std::shared_ptr<GameObject>& inObject, std::shared_ptr<GameObject>& inParent)
	{
		m_sceneOutliner->onLevelObjectDetached(inObject, inParent);
	}

}
