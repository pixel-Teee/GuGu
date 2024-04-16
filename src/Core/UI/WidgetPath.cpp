#include <pch.h>

#include "WidgetPath.h"
#include "Widget.h"
#include "ArrangedWidget.h"

namespace GuGu {
	WidgetPath::WidgetPath()
	{
	}
	WidgetPath::WidgetPath(const std::vector<std::shared_ptr<Widget>>& widgets)
		: m_widgets(ArrangedWidgetArray::hittestFromArray(widgets))
	{
		
	}
	WidgetPath WidgetPath::pathDownTo(std::shared_ptr<Widget> markerWidget) const
	{
		WidgetPath newWidgetPath;
		int32_t widgetNumber = m_widgets.getArrangedWidgetsNumber();
		bool bCopiedMarker = false;//是否复制
		for (int32_t i = 0; i < widgetNumber && !bCopiedMarker; ++i)
		{
			newWidgetPath.m_widgets.pushWidget(m_widgets[i]->getWidgetGeometry(), m_widgets[i]->getWidget());
			bCopiedMarker = m_widgets[i]->getWidget() == markerWidget;
		}
		return newWidgetPath;
	}
	WeakWidgetPath::WeakWidgetPath(const WidgetPath& inWidgetPath)
	{
		int32_t widgetNumber = inWidgetPath.m_widgets.getArrangedWidgetsNumber();
		for(int32_t i = 0; i < widgetNumber; ++i)
		{
			m_widgets.push_back(std::weak_ptr<Widget>(inWidgetPath.m_widgets[i]->getWidget()));
		}
	}
	void WeakWidgetPath::toWidgetPath(WidgetPath& widgetPath) const
	{
		for (int32_t i = 0; i < m_widgets.size(); ++i)
		{
			widgetPath.m_widgets.pushWidget(m_widgets[i].lock()->getWidgetGeometry(), m_widgets[i].lock());
		}
	}
	std::weak_ptr<Widget> WeakWidgetPath::getLastWidget() const
	{
		return m_widgets[m_widgets.size() - 1];
	}
	bool WeakWidgetPath::isEmpty() const
	{
		return m_widgets.empty();
	}
	void WeakWidgetPath::clear()
	{
		m_widgets.clear();
	}
}