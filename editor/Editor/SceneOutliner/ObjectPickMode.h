#pragma once

#include "ObjectModeInteractive.h"
#include "SceneOutlinerFwd.h" //OnSceneOutlinerItemPicker

namespace GuGu {
	class ObjectPickMode : public ObjectModeInteractive
	{
	public:
		ObjectPickMode(SceneOutlinerNameSpace::SceneOutliner* inSceneOutliner, OnSceneOutlinerItemPicker onItemPicked);

		virtual ~ObjectPickMode();

		virtual void onItemSelectionChanged(SceneOutlinerNameSpace::TreeItemPtr item, SelectInfo::Type selectionType, const SceneOutlinerNameSpace::SceneOutlinerItemSelection& selection) override;

	private:
		OnSceneOutlinerItemPicker m_onItemPicked;
	};
}