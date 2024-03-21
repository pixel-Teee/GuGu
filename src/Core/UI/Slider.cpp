#include <pch.h>

#include "Slider.h"
#include "Brush.h"
#include "ElementList.h"
#include "StyleSet.h"

#include <Application/Application.h>

namespace GuGu {
	Slider::Slider()
		: m_sliderStyle(StyleSet::getStyle()->getStyle<SliderStyle>("slider"))
	{
		m_orientation = Orientation::Horizontal;
		m_valueAttribute = 0.0f;
		m_minValue = 0.0f;
		m_maxValue = 1.0f;
	}
	Slider::~Slider()
	{
	}
	void Slider::init(const BuilderArguments& arguments)
	{
	}
	uint32_t Slider::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		//滑条分配的宽高
		const float allottedWidth = m_orientation == Orientation::Horizontal ? allocatedGeometry.getLocalSize().x : allocatedGeometry.getLocalSize().y;
		const float allottedHeight = m_orientation == Orientation::Horizontal ? allocatedGeometry.getLocalSize().y : allocatedGeometry.getLocalSize().x;

		float handleRotation;
		math::float2 handleTopLeftPoint;
		math::float2 sliderStartPoint;
		math::float2 sliderEndPoint;

		//计算slider的几何，当它是水平滑条一样，后面会旋转它，如果它是垂直的话
		const math::float2 handleSize = math::float2(getThumbImage()->m_actualSize.x, getThumbImage()->m_actualSize.y);
		const math::float2 halfHandleSize = 0.5f * handleSize;
		const float indentation = 0.0f;//缩进

		//保证滑条不会超出滑条长度
		const float sliderPercent = std::clamp(getNormalizedValue(), 0.0f, 1.0f);
		const float sliderLength = allottedWidth - (indentation + handleSize.x);
		const float sliderHandleOffset = sliderPercent * sliderLength;
		const float sliderY = 0.5f * allottedHeight;

		handleRotation = 0.0f;
		//滑块的左上角
		handleTopLeftPoint = math::float2(sliderHandleOffset + (0.5f * indentation), sliderY - halfHandleSize.y);

		sliderStartPoint = math::float2(halfHandleSize.x, sliderY);
		sliderEndPoint = math::float2(allottedWidth - halfHandleSize.x, sliderY);

		WidgetGeometry sliderGeometry = allocatedGeometry;

		//旋转滑条如果它是垂直的，0在最下面，1在最上面
		if (m_orientation == Orientation::Vertical)
		{
			//暂时不处理垂直的
		}

		//绘制滑条块
		auto barTopLeft = math::float2(sliderStartPoint.x, sliderStartPoint.y - m_sliderStyle->barThickNess * 0.5f);
		auto barSize = math::float2(sliderEndPoint.x - sliderStartPoint.x, m_sliderStyle->barThickNess);
		auto barImage = getBarImage();
		auto thumbImage = getThumbImage();
		ElementList::addBoxElement(elementList,
			allocatedGeometry.getChildGeometry(barSize, barTopLeft, allocatedGeometry.getAccumulateTransform()),
			math::float4(1.0f),
			barImage,
			layer
		);

		++layer;

		ElementList::addBoxElement(elementList,
			allocatedGeometry.getChildGeometry(handleSize, handleTopLeftPoint, allocatedGeometry.getAccumulateTransform()),
			math::float4(1.0f),
			thumbImage,
			layer
		);

		return layer;
	}
	math::float2 Slider::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 sliderDesiredSize(16.0f, 16.0f);

		if (m_sliderStyle == nullptr)
			return sliderDesiredSize;

		const float thickness = std::max(m_sliderStyle->barThickNess,
			(float)m_sliderStyle->m_normalThumbImage->m_actualSize.y);

		return math::float2(sliderDesiredSize.x, thickness);
	}
	Reply Slider::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		return Reply::Unhandled().captureMouse(shared_from_this());
	}
	Reply Slider::OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		return Reply::Unhandled().releaseMouseCapture();
	}
	Reply Slider::OnMouseMove(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (Application::getApplication()->getCaptorWidget() == shared_from_this())
		{
			//m_valueAttribute.Set(0.6);
			commitValue(positionToValue(geometry, inMouseEvent.m_screenSpacePosition));
		}

		return Reply::Unhandled();
	}
	float Slider::getNormalizedValue() const
	{
		if (m_maxValue == m_minValue)
		{
			return 1.0f;
		}
		else
		{
			return (m_valueAttribute.Get() - m_minValue) / (m_maxValue - m_minValue);
		}
	}
	std::shared_ptr<Brush> Slider::getThumbImage() const
	{
		return m_sliderStyle->m_normalThumbImage;
	}
	std::shared_ptr<Brush> Slider::getBarImage() const
	{
		return m_sliderStyle->m_normalBarImage;
	}
	void Slider::commitValue(float newValue)
	{
		m_valueAttribute.Set(newValue);

		if(m_onValueChanged)
			m_onValueChanged(newValue);
	}
	float Slider::positionToValue(const WidgetGeometry& geometry, const math::float2& absolutePosition)
	{
		const math::float2 localPosition = geometry.absoluteToLocal(absolutePosition);

		const float indentation = getThumbImage()->m_actualSize.x;
		const float halfIndentation = 0.5f * indentation;

		float denominator = geometry.mLocalSize.x - indentation;
		float relativeValue = (denominator != 0.0f) ? (localPosition.x - halfIndentation) / denominator : 0.0f;
		
		relativeValue = std::clamp(relativeValue, 0.0f, 1.0f) * (m_maxValue - m_minValue) + m_minValue;

		return relativeValue;
	}
}