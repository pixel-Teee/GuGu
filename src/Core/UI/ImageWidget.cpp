#include <pch.h>

#include "ImageWidget.h"

#include "Brush.h"

#include "ElementList.h"

#include "Style.h"

namespace GuGu {
	ImageWidget::ImageWidget()
		: m_imageBursh(Style::getStyle()->getBrush("fun"))
	{
	}
	ImageWidget::~ImageWidget()
	{
	}
	void ImageWidget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_imageBursh, layer);
	}
	math::double2 ImageWidget::ComputeFixedSize()
	{
		return math::double2(m_imageBursh->m_actualSize.x, m_imageBursh->m_actualSize.y);
	}

}