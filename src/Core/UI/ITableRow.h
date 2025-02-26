#pragma once

#include "ITypedTableView.h"//SelectionMode::Type

namespace GuGu {
	class Widget;
	class ITableRow
	{
	public:

		//当 item - widget 的 关系建立的时候，会调用这个函数
		virtual void initializeRow() = 0;

		virtual void resetRow() = 0;

		virtual void setIndexInList(int32_t inIndexInList) = 0;

		virtual std::shared_ptr<Widget> asWidget() = 0;

		virtual int32_t getIndentLevel() const = 0;

		virtual std::vector<int32_t> getWiresNeededByDepth() const = 0;

		virtual bool isLastChild() const = 0;

		virtual int32_t doesItemHaveChildren() const = 0;

		virtual bool isItemExpanded() const = 0;

		virtual void toggleExpansion() = 0;

		virtual SelectionMode::Type getSelectionMode() const = 0;
	};
}