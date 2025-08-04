#include <pch.h>

#include "ITreeItem.h"

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{

		TreeItemPtr ITreeItem::getParent() const
		{
			return m_parent.lock();
		}

		void ITreeItem::addChild(TreeItemPtr child)
		{
			child->m_parent = shared_from_this();
			m_children.push_back(std::move(child));
		}

		void ITreeItem::removeChild(const TreeItemPtr& child)
		{
			//find it
			int32_t pos = -1;
			for (int32_t i = 0; i < m_children.size(); ++i)
			{
				if (m_children[i].lock() == child)
				{
					pos = i;
					break;
				}
			}
			if (pos != -1)
			{
				m_children.erase(m_children.begin() + pos);
				child->m_parent.reset();
			}
			
		}

		const std::vector<std::weak_ptr<SceneOutlinerNameSpace::ITreeItem>> ITreeItem::getChildren() const
		{
			return m_children;
		}

	}
	

}