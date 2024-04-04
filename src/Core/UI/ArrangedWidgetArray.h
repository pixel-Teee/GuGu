#pragma once

#include <vector>

#include "Visibility.h"

namespace GuGu {
	class Widget;
	class WidgetGeometry;
	class ArrangedWidget;
	class ArrangedWidgetArray
	{
	public:
		ArrangedWidgetArray(Visibility inVisibilityFilter);

		virtual ~ArrangedWidgetArray();

		void pushWidget(Visibility visiblityOverride, const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget);

		void pushWidget(const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget);

		std::shared_ptr<ArrangedWidget> getArrangedWidget(uint32_t index);

		uint32_t getArrangedWidgetsNumber();

		bool accepts(Visibility inVisibility) const;
	private:

		std::vector<std::shared_ptr<ArrangedWidget>> m_widgets;

		Visibility m_visibilityFilter;
	};
}