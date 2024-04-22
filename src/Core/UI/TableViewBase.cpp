#include <pch.h>

#include "TableViewBase.h"

namespace GuGu {
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