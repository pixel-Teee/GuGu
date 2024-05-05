#include <pch.h>

#include "ColorBlock.h"
#include "ElementList.h"

namespace GuGu {
	ColorBlock::ColorBlock()
		: m_color(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
		, m_gradientCornerRaidus(math::float4(0.0f, 0.0f, 0.0f, 0.0f))
		, m_colorBlockSize(math::float4(16.0f, 16.0f))
		, m_alphaDisplayMode(ColorBlockAlphaDisplayMode::Combined)
		, m_colorIsHsv(false)
		, m_showBackgroundForAlpha(false)
		, m_bUseSRGB(true)
	{
	}
	ColorBlock::~ColorBlock()
	{
	}
	void ColorBlock::init(const BuilderArguments& arguments)
	{
		m_color = arguments.mColor;
		m_alphaBackgroundBrush = arguments.mAlphaBackgroundBrush.Get();
		m_gradientCornerRaidus = arguments.mcornerRadius;//圆角半径
		m_colorBlockSize = arguments.mSize;
		m_mouseButtonDownHandler = arguments.mOnMouseButtonDown;
		m_alphaDisplayMode = arguments.malphaDisplayMode;
		m_colorIsHsv = arguments.mcolorIsHsv;
		m_showBackgroundForAlpha = arguments.mshowBackgroundForAlpha;
		m_bUseSRGB = arguments.museSRGB;
	}
	uint32_t ColorBlock::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ColorBlockAlphaDisplayMode displayMode = m_alphaDisplayMode.Get();
		bool bIgnoreAlpha = displayMode == ColorBlockAlphaDisplayMode::Ignore;
		math::float4 inColor = m_color.Get();

		if (m_colorIsHsv.Get())
		{
			//from hsv to rgb
			inColor = math::HSVtoRGB(inColor);
		}

		if (m_showBackgroundForAlpha.Get() && inColor.w < 1.0f && !bIgnoreAlpha)
		{
			//ElementList::addBoxElement(elementList, allocatedGeometry, m_alphaBackgroundBrush.get()->m_tintColor, m_alphaBackgroundBrush, layer);
		}

		std::vector<GradientStop> gradientStop;

		switch (displayMode)
		{
		case GuGu::ColorBlockAlphaDisplayMode::Combined:
			makeSection(gradientStop, math::float2(0.0f, 0.0f), allocatedGeometry.getLocalSize(), inColor, false);
			break;
		case GuGu::ColorBlockAlphaDisplayMode::Separete:
			makeSection(gradientStop, math::float2(0.0f, 0.0f), allocatedGeometry.getLocalSize() * 0.5f, inColor, false);//不忽略透明度
			makeSection(gradientStop, allocatedGeometry.getLocalSize() * 0.5f, allocatedGeometry.getLocalSize(), inColor, true);//忽略透明度
			break;
		case GuGu::ColorBlockAlphaDisplayMode::Ignore:
			makeSection(gradientStop, math::float2(0.0f, 0.0f), allocatedGeometry.getLocalSize(), inColor, false);
			break;
		}

		ElementList::addGradientElement(elementList, allocatedGeometry, gradientStop, (allocatedGeometry.getLocalSize().x > allocatedGeometry.getLocalSize().y) ? Orientation::Vertical : Orientation::Horizontal,
			m_gradientCornerRaidus.Get(), layer + 1);
	
		return layer + 1;
	}
	math::float2 ColorBlock::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return m_colorBlockSize.Get();
	}
	Reply ColorBlock::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (m_mouseButtonDownHandler)
		{
			return m_mouseButtonDownHandler(myGeometry, inMouseEvent);
		}
		else
		{
			return Reply::Unhandled();
		}
	}
	void ColorBlock::makeSection(std::vector<GradientStop>& outGradientStop, math::float2 startPt, math::float2 endPt, math::float4 color, bool bIgnoreAlpha) const
	{
		//决定它是不是 hdr
		const float maxRGB = std::max(color.x, std::max(color.y, color.z));

		if (maxRGB > 1.0f)
		{
			const float alpha = bIgnoreAlpha ? 1.0f : color.w;
			math::float4 normalizedLinearColor = color / maxRGB;
			normalizedLinearColor.w = alpha;
			const math::float4 drawNormalizedColor = normalizedLinearColor;//todo:以后根据 bUseSRGB 来处理这里

			math::float4 clampedLinearColor = color;
			clampedLinearColor.w = alpha * maxRGB;
			const math::float4 drawClampedColor = clampedLinearColor;

			outGradientStop.push_back(GradientStop(startPt, drawNormalizedColor));
			outGradientStop.push_back(GradientStop((startPt + endPt) * 0.5f, drawClampedColor));
			outGradientStop.push_back(GradientStop(endPt, drawNormalizedColor));
		}
		else
		{
			math::float4 drawColor = color;
			if (bIgnoreAlpha)
			{
				drawColor.w = 255.0f;
			}

			outGradientStop.push_back(GradientStop(startPt, drawColor));
			//outGradientStop.push_back(GradientStop((startPt + endPt) * 0.5f, drawClampedColor));
			outGradientStop.push_back(GradientStop(endPt, drawColor));
		}
	}
}