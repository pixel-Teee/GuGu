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
	ArrangedWidget& ArrangedWidgetArray::back()
	{
		return *m_widgets.back();
	}
	ArrangedWidget& ArrangedWidgetArray::back() const
	{
		return *m_widgets.back();
	}
	bool ArrangedWidgetArray::accepts(Visibility inVisibility) const
	{
		return Visibility::doesVisibilityPassFilter(inVisibility, m_visibilityFilter);
	}
	ArrangedWidgetArray ArrangedWidgetArray::hittestFromArray(const std::vector<std::shared_ptr<Widget>>& widgets, const WidgetGeometry& offsetGeometry)
	{
		ArrangedWidgetArray temp(Visibility::All);
		temp.m_widgets.reserve(widgets.size());
		for (int32_t i = 0; i < widgets.size(); ++i)
		{
			WidgetGeometry newGeometry = widgets[i]->getWidgetGeometry().getOffsetGeometry(offsetGeometry.getAbsolutePosition());//追加窗口的左上角在屏幕坐标中的位置
			std::shared_ptr<ArrangedWidget> arrangedWidget = std::make_shared<ArrangedWidget>(newGeometry, widgets[i]);
			temp.m_widgets.push_back(arrangedWidget);
		}
		return temp;
	}
}