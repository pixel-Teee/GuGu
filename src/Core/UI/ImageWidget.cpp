#include <pch.h>

#include "ImageWidget.h"

#include "Brush.h"
#include "CoreStyle.h"
#include "ElementList.h"

namespace GuGu {
	ImageWidget::ImageWidget()
		: m_imageBursh(CoreStyle::getStyleSet()->getBrush("fun"))
	{
	}
	ImageWidget::~ImageWidget()
	{
	}
	void ImageWidget::init(const BuilderArguments& arguments)
	{
		//if(arguments.mbrush.Get())
		m_imageBursh = arguments.mbrush;
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}
	uint32_t ImageWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		if (m_imageBursh.Get()->m_drawAs != BrushDrawType::NoDrawType)
		{
			ElementList::addBoxElement(elementList, allocatedGeometry, m_imageBursh.Get()->m_tintColor, m_imageBursh.Get(), layer);
		}		

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