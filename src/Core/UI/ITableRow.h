#pragma once

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
	};
}