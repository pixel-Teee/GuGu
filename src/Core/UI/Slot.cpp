#include <pch.h>

#include "Slot.h"

namespace GuGu {

	SlotBase::SlotBase(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment, Padding inPadding)
		: m_childWidget(nullptr)
		, m_horizontalAlignment(inHorizontalAlignment)
		, m_verticalAlignment(inVerticalAlignment)
		, m_padding(inPadding)
	{
	}
	SlotBase::~SlotBase()
	{
	}
	std::shared_ptr<Widget> SlotBase::getChildWidget() const
	{
		return m_childWidget;
	}
	void SlotBase::setChildWidget(std::shared_ptr<Widget> widget)
	{
		m_childWidget = widget;
	}
	Padding SlotBase::getPadding() const
	{
		return m_padding;
	}
	HorizontalAlignment SlotBase::getHorizontalAlignment() const
	{
		return m_horizontalAlignment;
	}
	VerticalAlignment SlotBase::getVerticalAlignment() const
	{
		return m_verticalAlignment;
	}
	void SlotBase::setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
	{
		m_horizontalAlignment = horizontalAlignment;
	}
	void SlotBase::setVerticalAlignment(VerticalAlignment verticalAlignment)
	{
		m_verticalAlignment = verticalAlignment;
	}
	void SlotBase::setPadding(Padding newPadding)
	{
		m_padding = newPadding;
	}

}