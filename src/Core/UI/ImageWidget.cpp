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
	void ImageWidget::init(const BuilderArguments& arguments)
	{
		if(arguments.mbrush.Get())
			m_imageBursh = arguments.mbrush;
	}
	uint32_t ImageWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_imageBursh.Get(), layer);

		return layer;
	}
	math::double2 ImageWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::double2(m_imageBursh.Get()->m_actualSize.x, m_imageBursh.Get()->m_actualSize.y);
	}

}