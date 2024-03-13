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
		m_widgetClipping = arguments.mClip;
	}
	uint32_t ImageWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_imageBursh.Get(), layer);

		return layer;
	}
	GuGu::math::float2 ImageWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_imageBursh.Get())
			return math::float2(m_imageBursh.Get()->m_actualSize.x, m_imageBursh.Get()->m_actualSize.y);
		else
			return math::float2(0.0f, 0.0f);
	}

}