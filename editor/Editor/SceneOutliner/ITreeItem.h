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

			virtual TreeItemPtr findParent(const TreeItemMap& existingItems) const = 0;

			virtual TreeItemPtr getParent() const;

			virtual TreeItemPtr createParent() const = 0;

			virtual TreeItemID getID() const = 0;//索引 game object 用的

			virtual void addChild(TreeItemPtr child);

			virtual void removeChild(const TreeItemPtr& child);

			const std::vector<std::weak_ptr<ITreeItem>> getChildren() const;
		
		//todo:add protected
			mutable std::weak_ptr<ITreeItem> m_parent;

			mutable std::vector<std::weak_ptr<ITreeItem>> m_children;
		};
	}
}