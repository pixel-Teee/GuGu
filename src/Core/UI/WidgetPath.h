#pragma once

#include <vector>

#include "WidgetGeometry.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	class Widget;
	//widget path 的第一个参数是 window widget
	class WidgetPath
	{
	public:
		WidgetPath();

		WidgetPath(const std::vector<std::shared_ptr<Widget>>& widgets, const WidgetGeometry& offsetGeometry);

		//获取从 path 的根到 marker widget 的路径
		WidgetPath pathDownTo(std::shared_ptr<Widget> markerWidget) const;
		
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
	private:
		std::vector<std::weak_ptr<Widget>> m_widgets;

		WidgetGeometry m_offsetGeometry;
	};
}