#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include "TreeItem.h"

namespace GuGu {
	class Brush;
	class AssetTreeItem : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<AssetTreeItem>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			//这个item所表示的数据
			ARGUMENT_VALUE(std::shared_ptr<TreeItem>, treeItem)

			ARGUMENT_ATTRIBUTE(bool, isItemExpanded)
		};

		void init(const BuilderArguments& arguments);

		virtual ~AssetTreeItem();

		std::shared_ptr<Brush> getFolderIcon() const;

		GuGuUtf8Str getNameText() const;
	private:
		//这个项的数据
		std::weak_ptr<TreeItem> m_treeItem;

		std::shared_ptr<Widget> m_parentWidget;

		//这个项是否有孩子，并且已经展开
		Attribute<bool> m_isItemExpanded;

		std::shared_ptr<Brush> m_folderOpenBrush;
		std::shared_ptr<Brush> m_folderClosedBrush;
	};
}