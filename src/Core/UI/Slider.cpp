#include <pch.h>

#include "Slider.h"
#include "Brush.h"
#include "ElementList.h"
#include "CoreStyle.h"

#include <Application/Application.h>
#include <Core/Timer.h>

namespace GuGu {
	Slider::Slider()
		: m_sliderStyle(CoreStyle::getStyleSet()->getStyle<SliderStyle>("slider"))
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
		m_orientation = arguments.morientation;
		m_minValue = arguments.mMinValue;
		m_maxValue = arguments.mMaxValue;
		m_onValueChanged = arguments.mOnValueChanged;
		m_isFocusable = arguments.mIsFocusable;
		m_visibilityAttribute = arguments.mVisibility;
		m_sliderBarColor = arguments.msliderBarColor;
		m_valueAttribute = arguments.mValue;
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
		const float sliderPercent = std::clamp(getNormalizedValue(), 0.0f, 1.0f);//滑条的数值百分比
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
			//sliderGeometry = sliderGeometry.getChildGeometry(math::float2(allottedWidth, allottedHeight), math::float2(0.0f, 0.0f), sliderGeometry.getAccumulateTransform());
			math::affine2 renderTransform;

			math::float2 translation = math::float2(-allottedWidth, 0);
			
			//float time = Application::getApplication()->getTimer()->GetDeltaTime();
			//
			//static float totalTime = 0;
			//totalTime += time * 20.0f;

			float Radians = math::radians(-90.0f);
			math::float2 quat2D = math::float2(std::cosf(Radians), std::sinf(Radians));

			//transform point
			math::float2 rotatedTranslation = math::float2(translation.x * quat2D.x - translation.y * quat2D.y, translation.x * quat2D.y + translation.y * quat2D.x);

			//todo:这里之后需要简化，封装成函数
			renderTransform = math::affine2(math::float2x2(quat2D.x, quat2D.y, -quat2D.y, quat2D.x), rotatedTranslation);

			sliderGeometry = allocatedGeometry.getChildGeometry(
				math::float2(allottedWidth, allottedHeight),
				math::affine2::identity(),
				renderTransform,
				math::float2(0, 0) //pivot
			);
		}

		//绘制slider
		auto barTopLeft = math::float2(sliderStartPoint.x, sliderStartPoint.y - m_sliderStyle->barThickNess * 0.5f);
		auto barSize = math::float2(sliderEndPoint.x - sliderStartPoint.x, m_sliderStyle->barThickNess);
		auto barImage = getBarImage();
		auto thumbImage = getThumbImage();
		ElementList::addBoxElement(elementList,
			sliderGeometry.getChildGeometry(barSize, barTopLeft),
			barImage->getTintColor() * m_sliderBarColor.Get().toFloat4(),
			barImage,
			layer
		);

		++layer;

		//绘制handle
		ElementList::addBoxElement(elementList,
			sliderGeometry.getChildGeometry(handleSize, handleTopLeftPoint),
			thumbImage->getTintColor(),
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

		if (m_orientation == Orientation::Vertical)
		{
			return math::float2(thickness, sliderDesiredSize.y);
		}

		return math::float2(sliderDesiredSize.x, thickness);
	}
	Reply Slider::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
		{
			return Reply::Handled().captureMouse(shared_from_this());
		}
		return Reply::Unhandled();
	}
	Reply Slider::OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
		{
			return Reply::Handled().releaseMouseCapture();
		}
		return Reply::Unhandled();
	}
	Reply Slider::OnMouseMove(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (Application::getApplication()->getCaptorWidget() == shared_from_this())
		{
			//m_valueAttribute.Set(0.6);
			commitValue(positionToValue(geometry, inMouseEvent.m_screenSpacePosition));
			return Reply::Handled();
		}

		return Reply::Unhandled();
	}
	bool Slider::supportsKeyboardFocus() const
	{
		return m_isFocusable;
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

		float denominator;
		float relativeValue;

		const float indentation = getThumbImage()->m_actualSize.x;
		const float halfIndentation = 0.5f * indentation;

		if (m_orientation == Orientation::Horizontal)
		{
			denominator = geometry.mLocalSize.x - indentation;
			relativeValue = (denominator != 0.0f) ? (localPosition.x - halfIndentation) / denominator : 0.0f;
		}
		else
		{
			denominator = geometry.mLocalSize.y - indentation;
			relativeValue = (denominator != 0.0f) ? (geometry.mLocalSize.y - localPosition.y - halfIndentation) / denominator : 0.0f;
		}

		relativeValue = std::clamp(relativeValue, 0.0f, 1.0f) * (m_maxValue - m_minValue) + m_minValue;

		return relativeValue;
	}
	bool Slider::isInteractable() const
	{
		return true;
	}
}