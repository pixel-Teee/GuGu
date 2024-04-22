#pragma once

#include "Widget.h"
#include "IScrollableWidget.h"
#include "BasicElement.h"
#include "HeaderRow.h"
#include "ScrollBar.h"
#include "Style.h"

namespace GuGu {
	using OnTableViewScrolled = std::function<void(double)>;//从 item 列表开始的 scroll offset

	enum class ListItemAlignment : uint8_t
	{
		//item 沿着线均匀地分布(在item之间添加任何额外的空间作为填充)
		EvenlyDistributed,

		//item 沿着线均匀地分布(任何额外的空间都用于按比例放大item的大小)
		EvenlySize,

		//item 沿着线均匀地分布(任何额外的空间都用于按比例放大item的宽度)
		EvenlyWide,

		LeftAligned,

		RightAligned,

		CenterAligned,

		Fill
	};
	class TableViewBase : public Widget, public IScrollableWidget
	{
	public:
		//创建构成list的child widgets
		void constructChildren(const Attribute<float>& inItemWidth,
			const Attribute<float>& inItemHeight,
			const Attribute< ListItemAlignment>& inItemAlignment,
			const std::shared_ptr<HeaderRow>& inHeaderRow,
			const std::shared_ptr<ScrollBar>& inScrollBar,
			Orientation inScrollOrientation,
			const OnTableViewScrolled& inOnTableViewScrolled,
			const ScrollBarStyle* inScrollBarStyle = nullptr);
	};
}