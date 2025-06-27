#include <pch.h>

#include "ITreeItem.h"

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{
		void ITreeItem::addChild(TreeItemPtr child)
		{
			child->m_parent = shared_from_this();
			m_children.push_back(std::move(child));
		}

		const std::vector<std::weak_ptr<SceneOutlinerNameSpace::ITreeItem>> ITreeItem::getChildren() const
		{
			return m_children;
		}

	}
	

}