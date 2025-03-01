#pragma once

#include "ObjectModeInteractive.h"

namespace GuGu {
	class ObjectBrowsingMode : public ObjectModeInteractive
	{
	public:
		ObjectBrowsingMode(SceneOutlinerNameSpace::SceneOutliner* inSceneOutliner);

		virtual ~ObjectBrowsingMode();

		virtual void onItemSelectionChanged(SceneOutlinerNameSpace::TreeItemPtr item, SelectInfo::Type selectionType, const SceneOutlinerNameSpace::SceneOutlinerItemSelection& selection) override;

	private:

	};
}