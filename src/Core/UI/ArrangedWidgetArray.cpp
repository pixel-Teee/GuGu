#include <pch.h>

#include "ArrangedWidgetArray.h"

#include "ArrangedWidget.h"

#include "Widget.h"

namespace GuGu {
	ArrangedWidgetArray::ArrangedWidgetArray()
		: m_visibilityFilter(Visibility::All)
	{
	}
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
	uint32_t ArrangedWidgetArray::getArrangedWidgetsNumber() const
	{
		return m_widgets.size();
	}
	const std::shared_ptr<ArrangedWidget> ArrangedWidgetArray::operator[](size_t index) const
	{
		return m_widgets[index];
	}
	bool ArrangedWidgetArray::accepts(Visibility inVisibility) const
	{
		return Visibility::doesVisibilityPassFilter(inVisibility, m_visibilityFilter);
	}
	ArrangedWidgetArray ArrangedWidgetArray::hittestFromArray(const std::vector<std::shared_ptr<Widget>>& widgets)
	{
		ArrangedWidgetArray temp(Visibility::All);
		temp.m_widgets.reserve(widgets.size());
		for (int32_t i = 0; i < widgets.size(); ++i)
		{
			std::shared_ptr<ArrangedWidget> arrangedWidget = std::make_shared<ArrangedWidget>(widgets[i]->getWidgetGeometry(), widgets[i]);
			temp.m_widgets.push_back(arrangedWidget);
		}
		return temp;
	}
}