#pragma once

namespace GuGu {
	class ITableRow
	{
	public:

		//当 item - widget 的 关系建立的时候，会调用这个函数
		virtual void initializeRow() {}

		virtual void resetRow() {}
	};
}