#include <pch.h>

#include "ElementList.h"

#include "Element.h"

#include "BoxElement.h"

namespace GuGu {
	ElementList::ElementList()
	{
	}
	ElementList::~ElementList()
	{
	}
	void ElementList::addBoxElement(std::shared_ptr<ElementList> elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush)
	{
		//generate a box element to element list
		elementList->m_elements.push_back(std::make_shared<BoxElement>(Element::ElementType::Box, widgetGeometry, color, brush));
	}
}