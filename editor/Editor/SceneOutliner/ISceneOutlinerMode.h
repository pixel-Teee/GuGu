#pragma once

#include "SceneOutlinerFwd.h"//TreeItemPtr
#include <Core/UI/BasicElement.h>//SelectInfo

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		class SceneOutliner;
		struct SceneOutlinerItemSelection;
	}
	
	class ISceneOutlinerMode
	{
	public:
		ISceneOutlinerMode(SceneOutlinerNameSpace::SceneOutliner* inSceneOutliner) : m_sceneOutliner(inSceneOutliner) { }

		virtual ~ISceneOutlinerMode() {}

		virtual void onItemSelectionChanged(SceneOutlinerNameSpace::TreeItemPtr item, SelectInfo::Type selectionType, const SceneOutlinerNameSpace::SceneOutlinerItemSelection& selection) {}

	protected:
		SceneOutlinerNameSpace::SceneOutliner* m_sceneOutliner;
	};
}