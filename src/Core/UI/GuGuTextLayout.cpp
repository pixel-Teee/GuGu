#include <pch.h>

#include "GuGuTextLayout.h"

namespace GuGu {
	GuGuTextLayout::GuGuTextLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle)
		: m_defaultTextStyle(inDefaultTextStyle)
	{
	}
	GuGuTextLayout::~GuGuTextLayout()
	{
	}
	const TextBlockStyle& GuGuTextLayout::getDefaultTextStyle() const
	{
		return m_defaultTextStyle;
	}
}