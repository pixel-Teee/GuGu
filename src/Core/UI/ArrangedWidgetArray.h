#pragma once

#include <vector>

namespace GuGu {
	class ArrangedWidget;
	class WidgetGeometry;
	class Widget;
	class ArrangedWidgetArray
	{
	public:
		ArrangedWidgetArray();

		virtual ~ArrangedWidgetArray();

		void pushWidget(const WidgetGeometry& widgetGeometry, std::shared_ptr<Widget> inWidget);

		std::shared_ptr<ArrangedWidget> getArrangedWidget(uint32_t index);

		uint32_t getArrangedWidgetsNumber();
	private:

		std::vector<std::shared_ptr<ArrangedWidget>> m_widgets;
	};
}