#pragma once

#include <vector>
#include "BasicElement.h"

namespace GuGu {
	namespace TableViewMode
	{
		enum Type
		{
			List,
			Tile,
			Tree
		};
	}

	namespace SelectionMode
	{
		enum Type
		{
			//没有东西可以选择
			None,
			//每次只能选择一个项
			Single,

			//每次只能选择一个项，可以当开关清除选择
			SingleToggle,

			//多个项可以同一时间选择
			Multi
		};
	}

	class ITableRow;
	class Widget;
	template<typename ItemType>
	class ITypedTableView
	{
	public:
		//std::vector 的 每个 true 位表示一个垂直的线，被 tree view 的深度所需要
		virtual const std::vector<int32_t>& privateGetWiresNeededByDepth(int32_t itemIndexInList) const = 0;

		virtual int32_t privateGetNestingDepth(int32_t itemIndexInList) const = 0;

		virtual bool privateIsLastChild(int32_t itemIndexInList) const = 0;

		virtual int32_t privateDoesItemHaveChildren(int32_t itemIndexInList) const = 0;

		virtual const ItemType* privateItemFromWidget(const ITableRow* theWidget) const = 0;

		virtual bool privateIsItemExpanded(const ItemType& theItem) const = 0;

		virtual void privateSetItemExpansion(ItemType theItem, bool bShouldBeExpanded) = 0;

		virtual bool privateIsItemSelected(const ItemType& theItem) const = 0;

		virtual void privateSetItemSelection(ItemType theItem, bool bShouldBeSelected, bool bWasUserDirected = false) = 0;

		virtual void privateClearSelection() = 0;

		virtual void privateSignalSelectionChanged(SelectInfo::Type selectInfo) = 0;

		virtual std::shared_ptr<Widget> asWidget() = 0;

		virtual std::vector<ItemType> getSelectedItems() const = 0;
	};
}