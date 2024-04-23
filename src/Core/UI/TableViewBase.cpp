#include <pch.h>

#include "TableViewBase.h"

namespace GuGu {
	TableViewDimensions::TableViewDimensions(Orientation inOrientation)
        : m_orientation(inOrientation)
	{
	}
	TableViewDimensions::TableViewDimensions(Orientation inOrientation, float x, float y)
        : TableViewDimensions(inOrientation, math::float2(x, y))
	{
	}
	TableViewDimensions::TableViewDimensions(Orientation inOrientation, const math::float2& size)
        : TableViewDimensions(inOrientation)
	{
        if (inOrientation == Orientation::Vertical)
        {
            m_lineAxis = size.x;
            m_scrollAxis = size.y;
        }
        else
        {
			m_lineAxis = size.y;
			m_scrollAxis = size.x;
        }
	}
    void TableViewBase::constructChildren(const Attribute<float>& inItemWidth, 
        const Attribute<float>& inItemHeight, 
        const Attribute<ListItemAlignment>& inItemAlignment, 
        const std::shared_ptr<HeaderRow>& inHeaderRow, 
        const std::shared_ptr<ScrollBar>& inScrollBar, 
        Orientation inScrollOrientation, 
        const OnTableViewScrolled& inOnTableViewScrolled, 
        const ScrollBarStyle* inScrollBarStyle)
    {
    }

}