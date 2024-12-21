#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include "TreeItem.h"

namespace GuGu {
	class AssetTreeItem : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<AssetTreeItem>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			//这个item所表示的数据
			ARGUMENT_VALUE(std::shared_ptr<TreeItem>, treeItem)
		};

		void init(const BuilderArguments& arguments);

		virtual ~AssetTreeItem();
	private:
		//这个项的数据
		std::weak_ptr<TreeItem> m_treeItem;

		std::shared_ptr<Widget> m_parentWidget;
	};
}