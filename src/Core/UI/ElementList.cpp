#include <pch.h>

#include "ElementList.h"
#include "Element.h"
#include "BoxElement.h"
#include "Brush.h"

namespace GuGu {
	ElementList::ElementList()
	{
	}
	ElementList::~ElementList()
	{
	}
	void ElementList::clear()
	{
		m_elements.clear();
		m_batches.clear();
	}
	void ElementList::addBoxElement(ElementList& elementList, const WidgetGeometry& widgetGeometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer)
	{
		//generate a box element to element list
		elementList.m_elements.push_back(std::make_shared<BoxElement>(Element::ElementType::Box, widgetGeometry, color, brush, layer));
	}
	void ElementList::generateBatches()
	{
		std::stable_sort(m_elements.begin(), m_elements.end(), [=](const std::shared_ptr<Element>& lhs, const std::shared_ptr<Element>& rhs) {
				return lhs->m_layer <= rhs->m_layer;
			});

		for (size_t i = 0; i < m_elements.size(); ++i)
		{
			switch (m_elements[i]->m_elementType)
			{
				case Element::ElementType::Box:
				{
					std::shared_ptr<BatchData> boxBatch = std::make_shared<BatchData>();
					generateBoxBatch(boxBatch, m_elements[i]);
					m_batches.push_back(boxBatch);
					break;
				}
			}
		}
	}
	const std::vector<std::shared_ptr<BatchData>>& ElementList::getBatches() const
	{
		return m_batches;
	}
	void ElementList::generateBoxBatch(std::shared_ptr<BatchData> boxBatch, std::shared_ptr<Element> element)
	{
		std::shared_ptr<BoxElement> boxElement = std::static_pointer_cast<BoxElement>(element);
		math::double2 absolutePosition = boxElement->m_geometry.getAbsolutePosition();
		math::double2 localSize = boxElement->m_geometry.getLocalSize();
		math::float4 color = boxElement->m_color;
		std::shared_ptr<Brush> brush = boxElement->m_brush;

		//math::float2 tiling(localSize.x / brush->m_actualSize.x, localSize.y / brush->m_actualSize.x);

		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x, brush->m_startUV.y, 1.0f, 1.0f), math::float2(0.0f, 0.0f), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x + brush->m_sizeUV.x, brush->m_startUV.y + brush->m_sizeUV.y, 1.0f, 1.0f), math::float2(localSize.x, localSize.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x, brush->m_startUV.y + brush->m_sizeUV.y, 1.0f, 1.0f), math::float2(0.0f, localSize.y), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));
		boxBatch->m_vertices.emplace_back(math::float4(brush->m_startUV.x + brush->m_sizeUV.x, brush->m_startUV.y, 1.0f, 1.0f), math::float2(localSize.x, 0.0f), color, math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(1);
		boxBatch->m_indices.emplace_back(2);
		boxBatch->m_indices.emplace_back(0);
		boxBatch->m_indices.emplace_back(3);
		boxBatch->m_indices.emplace_back(1);

		boxBatch->m_brush = brush;
	}
}