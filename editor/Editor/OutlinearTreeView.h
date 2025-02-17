#pragma once

#include <Core/UI/TreeView.h>
#include "ITreeItem.h"

namespace GuGu {
	namespace SceneOutlinear
	{
		typedef std::shared_ptr<ITreeItem> FTreeItemPtr;
		class OutlinearTreeView : public TreeView<FTreeItemPtr> //ITreeItem
		{
		public:

		protected:
		};
	}
}