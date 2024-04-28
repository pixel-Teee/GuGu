#pragma once

#include <vector>

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

	class ITableRow;
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

	};
}