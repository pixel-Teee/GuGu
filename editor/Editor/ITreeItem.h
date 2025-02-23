#pragma once

#include <memory>
#include <vector>

#include "SceneOutlinerFwd.h"
#include <Core/UI/TableRow.h>

namespace GuGu {
	class Widget;
	class ISceneOutliner;
	namespace SceneOutlinerNameSpace
	{
		struct ITreeItem : public std::enable_shared_from_this<ITreeItem>
		{
		public:
			virtual GuGuUtf8Str getDisplayString() const = 0;

			virtual std::shared_ptr<Widget> generateLabelWidget(ISceneOutliner& outliner, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& inRow) { return NullWidget::getNullWidget(); }
		protected:
			mutable std::weak_ptr<ITreeItem> m_parent;

			mutable std::vector<std::weak_ptr<ITreeItem>> m_children;


		};
	}
}