#pragma once

#include "Slot.h"
#include "Widget.h"

namespace GuGu {
	//排列儿子控件在一个垂直的列表，之间没有间隔
	//items 在这个 panel ，有一个统一的高度
	//也支持垂直地偏移它们的item
	class ListPanel : public Widget
	{
	public:
		ListPanel();

		virtual ~ListPanel();

		class PaneSlot : public Slot<PaneSlot>
		{
		public:
			PaneSlot(){}

			virtual ~PaneSlot() = default;
		};
	};
}