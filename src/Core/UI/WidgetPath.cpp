#include <pch.h>

#include "WidgetPath.h"
#include "Widget.h"
#include "ArrangedWidget.h"
#include "WindowWidget.h"

namespace GuGu {
	WidgetPath::WidgetPath()
	{
	}
	WidgetPath::WidgetPath(std::shared_ptr<WindowWidget> inTopLevelWindow, const ArrangedWidgetArray& inWidgetPath)
		: m_widgets(inWidgetPath)
		, m_offsetGeometry()
	{
	}
	WidgetPath::WidgetPath(const std::vector<std::shared_ptr<Widget>>& widgets, const WidgetGeometry& offsetGeometry)
		: m_widgets(ArrangedWidgetArray::hittestFromArray(widgets, offsetGeometry))
		, m_offsetGeometry(offsetGeometry)
	{
		
	}
	WidgetPath WidgetPath::pathDownTo(std::shared_ptr<Widget> markerWidget) const
	{
		WidgetPath newWidgetPath;
		int32_t widgetNumber = m_widgets.getArrangedWidgetsNumber();
		bool bCopiedMarker = false;//是否复制
		for (int32_t i = 0; i < widgetNumber && !bCopiedMarker; ++i)
		{
			newWidgetPath.m_widgets.pushWidget(m_widgets[i]->getWidget()->getWidgetGeometry(), m_widgets[i]->getWidget());
			newWidgetPath.m_offsetGeometry = m_offsetGeometry;
			bCopiedMarker = m_widgets[i]->getWidget() == markerWidget;
		}
		return newWidgetPath;
	}
	bool WidgetPath::searchForWidgetRecursively(const std::shared_ptr<Widget>& matcher, const ArrangedWidget& inCandidate, ArrangedWidgetArray& outReversedPath, Visibility visibilityFilter)
	{	
		ArrangedWidgetArray arrangedChildren(visibilityFilter);
		inCandidate.getWidget()->AllocationChildActualSpace(inCandidate.getWidgetGeometry(), arrangedChildren);

		for (int32_t childIndex = 0; childIndex < arrangedChildren.getArrangedWidgetsNumber(); ++childIndex)
		{
			const ArrangedWidget& someChild = *arrangedChildren.getArrangedWidget(childIndex);
			if (matcher == someChild.getWidget())
			{
				outReversedPath.pushWidget(WidgetGeometry(), someChild.getWidget());
				return true;
			}
			else if (searchForWidgetRecursively(matcher, someChild, outReversedPath, visibilityFilter))
			{
				outReversedPath.pushWidget(WidgetGeometry(), someChild.getWidget());
				return true;
			}
		}

		return false;	
	}
	ArrangedWidgetArray WidgetPath::generatePathToWidget(const std::shared_ptr<Widget>& matcher, const ArrangedWidget& fromWidget, Visibility visibility)
	{

		//从 from widget 找到一条到 matcher 的路径
		ArrangedWidgetArray pathResult(visibility);

		searchForWidgetRecursively(matcher, fromWidget, pathResult, visibility);

		pathResult.reverse();

		return pathResult;
	}
	bool WidgetPath::extendPathTo(const std::shared_ptr<Widget>& matcher, Visibility visibilityFilter)
	{

		const ArrangedWidget& lastWidget = m_widgets.back();

		ArrangedWidgetArray extension = generatePathToWidget(matcher, lastWidget, visibilityFilter);

		for (int32_t widgetIndex = 0; widgetIndex < extension.getArrangedWidgetsNumber(); ++widgetIndex)
		{
			m_widgets.pushWidget(WidgetGeometry(), extension.getArrangedWidget(widgetIndex)->getWidget());
		}

		return extension.getArrangedWidgetsNumber() > 0;//扩展成功，扩展到 matcher
	}
	bool WidgetPath::isValid() const
	{
		return m_widgets.getArrangedWidgetsNumber() > 0;
	}
	std::shared_ptr<WindowWidget> WidgetPath::getWindow() const
	{
		std::shared_ptr<WindowWidget> firstWidgetWindow = std::static_pointer_cast<WindowWidget>(m_widgets[0]->getWidget());

		return firstWidgetWindow;
	}
	bool WidgetPath::containsWidget(const Widget* widgetToFind) const
	{
		int32_t widgetNumber = m_widgets.getArrangedWidgetsNumber();
		for (int32_t widgetIndex = 0; widgetIndex < widgetNumber; ++widgetIndex)
		{
			if (m_widgets[widgetIndex]->getWidget().get() == widgetToFind)
			{
				return true;
			}
		}
		return false;
	}
	WeakWidgetPath::WeakWidgetPath(const WidgetPath& inWidgetPath)
	{
		int32_t widgetNumber = inWidgetPath.m_widgets.getArrangedWidgetsNumber();
		for(int32_t i = 0; i < widgetNumber; ++i)
		{
			m_widgets.push_back(std::weak_ptr<Widget>(inWidgetPath.m_widgets[i]->getWidget()));
		}
		m_offsetGeometry = inWidgetPath.m_offsetGeometry;
	}
	void WeakWidgetPath::toWidgetPath(WidgetPath& widgetPath) const
	{
		for (int32_t i = 0; i < m_widgets.size(); ++i)
		{
			if(m_widgets[i].lock()) //todo:这里可能要修复一下，因为 widget 可能在某处引用计数为0
				widgetPath.m_widgets.pushWidget(m_widgets[i].lock()->getWidgetGeometry().getOffsetGeometry(m_offsetGeometry.getAbsolutePosition()), m_widgets[i].lock());
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
	bool WeakWidgetPath::containsWidget(const Widget* someWidget) const
	{
		for (int32_t widgetIndex = 0; widgetIndex < m_widgets.size(); ++widgetIndex)
		{
			if (m_widgets[widgetIndex].lock().get() == someWidget)
			{
				return true;
			}
		}
		return false;
	}
}