#include <pch.h>

#include "ObjectBrowingMode.h"
#include "SceneOutliner.h"
#include <Editor/EditorViewportClient.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/World.h>


namespace GuGu {

	ObjectBrowsingMode::ObjectBrowsingMode(SceneOutlinerNameSpace::SceneOutliner* inSceneOutliner)
		: ObjectModeInteractive(inSceneOutliner)
	{

	}

	ObjectBrowsingMode::~ObjectBrowsingMode()
	{

	}

	void ObjectBrowsingMode::onItemSelectionChanged(SceneOutlinerNameSpace::TreeItemPtr item, SelectInfo::Type selectionType, const SceneOutlinerNameSpace::SceneOutlinerItemSelection& selection)
	{
		std::vector<GameObject*> selectGameObjects = selection.getData<GameObject*>();

		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		if (viewportClient)
		{
			viewportClient->broadcastGameObjectSelectionChanged(selectGameObjects, true);
		}

	}

}