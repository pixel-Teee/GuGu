#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/TableRow.h>
#include "ITreeItem.h"
//#include "SceneOutlinerFwd.h"
#include "SceneOutlinerStandaloneTypes.h"//class SceneOutlinerCommonLabelData

namespace GuGu {
	class GameObject;
	struct ObjectTreeItem;
	namespace SceneOutlinerNameSpace
	{
		struct ObjectTreeLabel : public SceneOutlinerCommonLabelData, public CompoundWidget
		{
			struct BuilderArguments : public Arguments<ObjectTreeLabel>
			{
				BuilderArguments() {}

				~BuilderArguments() = default;

				//nothing
			};

			void init(const BuilderArguments& arguments, ObjectTreeItem& objectItem, ISceneOutliner& sceneOutliner,
				const TableRow<TreeItemPtr>& inRow);

			GuGuUtf8Str getDisplayText() const;
		private:
			std::weak_ptr<ObjectTreeItem> m_treeItemPtr;
			std::weak_ptr<GameObject> m_objectPtr;
		};
	}
	
}