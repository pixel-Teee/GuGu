#pragma once

#include "WidgetGeometry.h"

namespace GuGu {
	class Widget;
	class ArrangedWidget
	{
	public:
		ArrangedWidget(WidgetGeometry widgetGeometry, std::shared_ptr<Widget> inWidget);

		virtual ~ArrangedWidget();

		std::shared_ptr<Widget> getWidget() const;

		const WidgetGeometry& getWidgetGeometry() const;
	private:
		WidgetGeometry m_widgetGeometry;
		std::shared_ptr<Widget> m_widget;
	};
}