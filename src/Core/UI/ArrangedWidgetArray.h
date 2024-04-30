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
		ArrangedWidgetArray();

		ArrangedWidgetArray(Visibility inVisibilityFilter);

		virtual ~ArrangedWidgetArray();

		void pushWidget(Visibility visiblityOverride, const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget);

		void pushWidget(const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget);

		std::shared_ptr<ArrangedWidget> getArrangedWidget(uint32_t index);

		uint32_t getArrangedWidgetsNumber() const;

		const std::shared_ptr<ArrangedWidget> operator[](size_t index) const;

		ArrangedWidget& back();

		ArrangedWidget& back() const;

		bool accepts(Visibility inVisibility) const;

		//从一堆widgets构造arranged widget array
		static ArrangedWidgetArray hittestFromArray(const std::vector<std::shared_ptr<Widget>>& widgets, const WidgetGeometry& offsetGeometry);

		void reverse()
		{
			std::reverse(m_widgets.begin(), m_widgets.end());
		}
	private:

		std::vector<std::shared_ptr<ArrangedWidget>> m_widgets;

		Visibility m_visibilityFilter;
	};
}