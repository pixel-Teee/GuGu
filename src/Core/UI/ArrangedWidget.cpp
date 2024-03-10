#include <pch.h>

#include "ArrangedWidget.h"

namespace GuGu {
	ArrangedWidget::ArrangedWidget(WidgetGeometry widgetGeometry, std::shared_ptr<Widget> inWidget)
		: m_widgetGeometry(widgetGeometry)
		, m_widget(inWidget)
	{
	}
	ArrangedWidget::~ArrangedWidget()
	{
	}
	std::shared_ptr<Widget> ArrangedWidget::getWidget() const
	{
		return m_widget;
	}
	const WidgetGeometry& ArrangedWidget::getWidgetGeometry() const
	{
		return m_widgetGeometry;
	}
}