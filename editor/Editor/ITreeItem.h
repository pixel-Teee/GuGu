#pragma once

#include <memory>
#include <vector>

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		struct ITreeItem : public std::enable_shared_from_this<ITreeItem>
		{
		protected:
			mutable std::weak_ptr<ITreeItem> m_parent;

			mutable std::vector<std::weak_ptr<ITreeItem>> m_children;


		};
	}
}