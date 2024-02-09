#pragma once

#include <vector>

#include <Renderer/MyMath.h>

namespace GuGu {
	class Element;
	class WidgetGeometry;
	class Brush;
	class ElementList
	{
	public:
		ElementList();

		virtual ~ElementList();

		static void addBoxElement(std::shared_ptr<ElementList> elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush);
	private:
		std::vector<std::shared_ptr<Element>> m_elements;
	};
}