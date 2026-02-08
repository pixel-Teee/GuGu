#include <pch.h>

#include "ObjectPickMode.h"
#include "SceneOutliner.h"
#include <Editor/EditorViewportClient.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/World.h>

namespace GuGu {

	ObjectPickMode::ObjectPickMode(SceneOutlinerNameSpace::SceneOutliner* inSceneOutliner, OnSceneOutlinerItemPicker onItemPicked)
		: ObjectModeInteractive(inSceneOutliner)
		, m_onItemPicked(onItemPicked)
	{

	}

	ObjectPickMode::~ObjectPickMode()
	{

	}

	void ObjectPickMode::onItemSelectionChanged(SceneOutlinerNameSpace::TreeItemPtr item, SelectInfo::Type selectionType, const SceneOutlinerNameSpace::SceneOutlinerItemSelection& selection)
	{
		//std::vector<GameObject*> selectGameObjects = selection.getData<GameObject*>();
		//
		//std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		//if (viewportClient)
		//{
		//	viewportClient->broadcastGameObjectSelectionChanged(selectGameObjects, true);
		//}
		auto selectedItems = m_sceneOutliner->getSelectedItems();
		if (selectedItems.size() > 0)
		{
			auto firstItem = selectedItems[0];
			//check can interact
			if (m_onItemPicked)
			{
				m_onItemPicked(firstItem);
			}
		}
	}

}