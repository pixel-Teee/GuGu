#include <pch.h>

#include "TreeItem.h"

namespace GuGu {
	std::shared_ptr<TreeItem> TreeItem::getChild(const GuGuUtf8Str& inChildForlderName) const
	{
		for (const std::shared_ptr<TreeItem>& child : m_children)
		{
			if (child->m_folderName == inChildForlderName)
				return child;
		}
		return nullptr;
	}

}