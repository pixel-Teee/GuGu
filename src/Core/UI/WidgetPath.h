#pragma once

#include <vector>

#include "WidgetGeometry.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	class Widget;
	//class ArrangedWidget;
	class WindowWidget;
	//widget path 的第一个参数是 window widget
	class WidgetPath
	{
	public:
		WidgetPath();

		WidgetPath(std::shared_ptr<WindowWidget> inTopLevelWindow, const ArrangedWidgetArray& inWidgetPath);

		WidgetPath(const std::vector<std::shared_ptr<Widget>>& widgets, const WidgetGeometry& offsetGeometry);

		//获取从 path 的根到 marker widget 的路径
		WidgetPath pathDownTo(std::shared_ptr<Widget> markerWidget) const;

		bool searchForWidgetRecursively(const std::shared_ptr<Widget>& matcher, const ArrangedWidget& inCandidate, ArrangedWidgetArray& outReversedPath,
			Visibility visibilityFilter);

		ArrangedWidgetArray generatePathToWidget(const std::shared_ptr<Widget>& matcher, const ArrangedWidget& fromWidget, Visibility visibility);

		bool extendPathTo(const std::shared_ptr<Widget>& matcher, Visibility visibilityFilter = Visibility::Visible);

		bool isValid() const;

		std::shared_ptr<WindowWidget> getWindow() const;

		bool containsWidget(const Widget* widgetToFind) const;

		std::shared_ptr<Widget> getLastWidget() const;
		
		ArrangedWidgetArray m_widgets;

		WidgetGeometry m_offsetGeometry;
	};

	class WeakWidgetPath
	{
	public:
		WeakWidgetPath(const WidgetPath& inWidgetPath = WidgetPath());

		void toWidgetPath(WidgetPath& widgetPath) const;

		std::weak_ptr<Widget> getLastWidget() const;

		bool isEmpty() const;

		void clear();

		bool containsWidget(const Widget* someWidget) const;
	
		std::vector<std::weak_ptr<Widget>> m_widgets;

		WidgetGeometry m_offsetGeometry;
	};
}