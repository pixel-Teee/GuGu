#include <pch.h>

#include "ImageWidget.h"

#include "Brush.h"
#include "StyleSet.h"
#include "ElementList.h"

namespace GuGu {
	ImageWidget::ImageWidget()
		: m_imageBursh(StyleSet::getStyle()->getBrush("fun"))
	{
	}
	ImageWidget::~ImageWidget()
	{
	}
	uint32_t ImageWidget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_imageBursh, layer);

		return layer;
	}
	math::double2 ImageWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::double2(m_imageBursh->m_actualSize.x, m_imageBursh->m_actualSize.y);
	}

}