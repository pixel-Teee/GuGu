#include <pch.h>

#include "ArrangedWidgetArray.h"

#include "ArrangedWidget.h"

namespace GuGu {
	ArrangedWidgetArray::ArrangedWidgetArray()
	{
	}
	ArrangedWidgetArray::~ArrangedWidgetArray()
	{
	}
	void ArrangedWidgetArray::pushWidget(const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget)
	{
		std::shared_ptr<ArrangedWidget> arrangedWidget = std::make_shared<ArrangedWidget>(widgetGeometry, inWidget);
		m_widgets.emplace_back(arrangedWidget);
	}
	std::shared_ptr<ArrangedWidget> ArrangedWidgetArray::getArrangedWidget(uint32_t index)
	{
		return m_widgets[index];
	}
	uint32_t ArrangedWidgetArray::getArrangedWidgetsNumber()
	{
		return m_widgets.size();
	}
}