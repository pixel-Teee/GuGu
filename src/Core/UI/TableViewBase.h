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

	//当计算table layout elements 的时候，抽象出X和Y的需求
	struct TableViewDimensions
	{
		TableViewDimensions(Orientation inOrientation);
		TableViewDimensions(Orientation inOrientation, float x, float y);
		TableViewDimensions(Orientation inOrientation, const math::float2& size);

		math::float2 toVector2d() const
		{
			return m_orientation == Orientation::Vertical ? math::float2(m_lineAxis, m_scrollAxis) : math::float2(m_scrollAxis, m_lineAxis);
		}

		TableViewDimensions operator+(const TableViewDimensions& other) const
		{
			return TableViewDimensions(m_orientation, toVector2d() + other.toVector2d());
		}

		Orientation m_orientation = Orientation::Vertical;

		//沿着 table view 滑动轴的维度 (当垂直的时候是Y，当水平的时候是X)
		float m_scrollAxis = 0.0f;
		//垂直于滑动轴的维度，沿着被创建的 items 的行，只对 tile views 有用
		float m_lineAxis = 0.0f;
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