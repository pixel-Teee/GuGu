#include <pch.h>

#include "Slot.h"

namespace GuGu {
	Slot::Slot()
		: m_childWidget(nullptr)
		, m_horizontalAlignment(HorizontalAlignment::Center)
		, m_verticalAlignment(VerticalAlignment::Center)
		, m_padding(Padding(0.0, 0.0, 0.0, 0.0))
	{
	}
	Slot::~Slot()
	{
	}
	std::shared_ptr<Widget> Slot::getChildWidget()
	{
		return m_childWidget;
	}
	void Slot::setChildWidget(std::shared_ptr<Widget> widget)
	{
		m_childWidget = widget;
	}
	Padding Slot::getPadding()
	{
		return m_padding;
	}
	HorizontalAlignment Slot::getHorizontalAlignment()
	{
		return m_horizontalAlignment;
	}
	VerticalAlignment Slot::getVerticalAlignment()
	{
		return m_verticalAlignment;
	}
	void Slot::setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
	{
		m_horizontalAlignment = horizontalAlignment;
	}
	void Slot::setVerticalAlignment(VerticalAlignment verticalAlignment)
	{
		m_verticalAlignment = verticalAlignment;
	}
	void Slot::setPadding(Padding newPadding)
	{
		m_padding = newPadding;
	}
}