#pragma once

#include "DetailTableRowBase.h"

namespace GuGu {
	class IMenu;
	class DetailLayoutBuilderImpl;
	class DetailCategoryTableRow : public DetailTableRowBase
	{
	public:
		struct BuilderArguments : public Arguments<DetailCategoryTableRow>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(GuGuUtf8Str, displayName)

			ARGUMENT_VALUE(std::shared_ptr<Widget>, headerContent)

			//UI_EVENT(OnContextMenuOpening, onContextMenuOpening)
		};

		void init(const BuilderArguments& inArgs, std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView, std::shared_ptr<DetailLayoutBuilderImpl> inDetailLayoutBuilderImpl);

		Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

		Reply rightClick();
	private:
		std::weak_ptr<DetailLayoutBuilderImpl> m_detailLayoutBuilderImpl;

		//右键弹出的菜单
		//OnContextMenuOpening m_onContextMenuOpening;

		std::shared_ptr<IMenu> m_menu;
	};
}