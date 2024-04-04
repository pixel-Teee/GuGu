#include <pch.h>

#include "ArrangedWidgetArray.h"

#include "ArrangedWidget.h"

#include "Widget.h"

namespace GuGu {
	ArrangedWidgetArray::ArrangedWidgetArray(Visibility inVisibilityFilter)
		: m_visibilityFilter(inVisibilityFilter)
	{
	}
	ArrangedWidgetArray::~ArrangedWidgetArray()
	{
	}
	void ArrangedWidgetArray::pushWidget(Visibility visiblityOverride, const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget)
	{
		if (accepts(visiblityOverride))
		{
			std::shared_ptr<ArrangedWidget> arrangedWidget = std::make_shared<ArrangedWidget>(widgetGeometry, inWidget);
			m_widgets.emplace_back(arrangedWidget);
		}	
	}
	void ArrangedWidgetArray::pushWidget(const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget)
	{
		if (accepts(inWidget->getVisibility()))
		{
			std::shared_ptr<ArrangedWidget> arrangedWidget = std::make_shared<ArrangedWidget>(widgetGeometry, inWidget);
			m_widgets.emplace_back(arrangedWidget);
		}	
	}
	std::shared_ptr<ArrangedWidget> ArrangedWidgetArray::getArrangedWidget(uint32_t index)
	{
		return m_widgets[index];
	}
	uint32_t ArrangedWidgetArray::getArrangedWidgetsNumber()
	{
		return m_widgets.size();
	}
	bool ArrangedWidgetArray::accepts(Visibility inVisibility) const
	{
		return Visibility::doesVisibilityPassFilter(inVisibility, m_visibilityFilter);
	}
}