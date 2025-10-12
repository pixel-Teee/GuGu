#pragma once

#include "ColorWheel.h"
#include "Style.h"
#include "CoreStyle.h"
#include "Element.h"
#include "ElementList.h"


namespace GuGu {
	ColorWheel::ColorWheel()
	{
	}
	ColorWheel::~ColorWheel()
	{
	}
	void ColorWheel::init(const BuilderArguments& arguments)
	{
		m_image = CoreStyle::getStyleSet()->getBrush("colorWheel.HueValueCircle");
		m_selectorImage = CoreStyle::getStyleSet()->getBrush("colorWheel.Selector");
		m_selectedColor = arguments.mSelectedColor;

		m_onMouseCaptureBegin = arguments.mOnMouseCaptureBegin;
		m_onMouseCaptureEnd = arguments.mOnMouseCaptureEnd;
		m_onValueChanged = arguments.mOnValueChanged;
	}
	uint32_t ColorWheel::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		const math::float2 selectorSize = math::float2(m_selectorImage->m_actualSize.x, m_selectorImage->m_actualSize.y);
		const math::float2 circleSize = allocatedGeometry.getLocalSize() - selectorSize;

		WidgetGeometry imageGeometry = allocatedGeometry.getChildGeometry(circleSize, 0.5f * selectorSize);
		ElementList::addBoxElement(elementList, imageGeometry, m_image->getTintColor(), m_image, layer);

		WidgetGeometry selectorGeometry = allocatedGeometry.getChildGeometry(selectorSize, 0.5f * (allocatedGeometry.getLocalSize() + calcRelativePositionFromCenter() * circleSize - selectorSize));
		ElementList::addBoxElement(elementList, selectorGeometry, m_selectorImage->getTintColor(), m_selectorImage, layer + 1);

		return layer;
	}
	math::float2 ColorWheel::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::int2 imageSize = m_image->m_actualSize + m_selectorImage->m_actualSize;
		return math::float2(imageSize.x, imageSize.y);
	}
	Reply ColorWheel::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (m_onMouseCaptureBegin)
		{
			m_onMouseCaptureBegin();		
		}

		if (!processMouseAction(myGeometry, inMouseEvent, false))
		{
			if (m_onMouseCaptureEnd)
				m_onMouseCaptureEnd();
			return Reply::Unhandled();
		}
		return Reply::Handled().captureMouse(shared_from_this());;

		//return Reply::Unhandled();
	}
	Reply ColorWheel::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (hasMouseCapture())
		{
			if (m_onMouseCaptureEnd)
			{
				m_onMouseCaptureEnd();
			}

			return Reply::Handled().releaseMouseCapture();
		}		
		
		return Reply::Unhandled();
	}
	Reply ColorWheel::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (!hasMouseCapture())
			return Reply::Unhandled();

		processMouseAction(myGeometry, inMouseEvent, true);

		return Reply::Handled();
	}
	math::float2 ColorWheel::calcRelativePositionFromCenter() const
	{
		float hue = m_selectedColor.Get().r;//r
		float saturation = m_selectedColor.Get().g;//g
		float angle = hue / 180.0f * 3.1415926535f;
		float radius = saturation;

		return math::float2(std::cos(angle), std::sin(angle)) * radius;
	}
	bool ColorWheel::processMouseAction(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent, bool bProcessWhenOutsideColorWheel)
	{
		const math::float2 localMouseCoordinate = myGeometry.absoluteToLocal(mouseEvent.m_screenSpacePosition);
		const math::float2 relativePositionFromCenter = (2.0f * localMouseCoordinate - myGeometry.getLocalSize()) / (myGeometry.getLocalSize() - math::float2(m_selectorImage->m_actualSize.x, m_selectorImage->m_actualSize.y));
		const float relativeRadius = std::sqrt(relativePositionFromCenter.x * relativePositionFromCenter.x + relativePositionFromCenter.y * relativePositionFromCenter.y);//size

		if (relativeRadius <= 1.0f || bProcessWhenOutsideColorWheel)
		{
			float angle = std::atan2(relativePositionFromCenter.y, relativePositionFromCenter.x);

			if (angle < 0.0f)
			{
				angle += 2.0f * 3.1415926535f;
			}

			math::float4 newColor = m_selectedColor.Get().toFloat4();
			{
				newColor.x = angle * 180.0f * (0.31830988618f);//INV_PI
				newColor.y = std::min(relativeRadius, 1.0f);
				newColor.z = 1.0f;//todo:移除这个
				newColor.w = 1.0f;//todo:移除这个
			}

			if (m_onValueChanged)
			{
				m_onValueChanged(newColor);
			}

			m_selectedColor = newColor;//todo:移除这个，当实现完 color picker 的时候，当 color picker 提交的时候，更新 m_selected color
		}

		return (relativeRadius <= 1.0f);
	}
}
