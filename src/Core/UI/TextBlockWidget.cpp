#include <pch.h>

#include "TextBlockWidget.h"

#include "FontCache.h"
#include "Style.h"
#include "CoreStyle.h"
#include "ElementList.h"
#include "PlainTextLayoutMarshaller.h"

namespace GuGu {
	TextBlockWidget::TextBlockWidget()
	{
		m_textStyle = CoreStyle::getStyleSet()->getStyle<TextBlockStyle>("normalText");

		m_textLayoutCache = std::make_unique<TextBlockLayout>(this, *m_textStyle, PlainTextLayoutMarshaller::Create());
	}
	TextBlockWidget::~TextBlockWidget()
	{
	}
	void TextBlockWidget::init(const BuilderArguments& arguments)
	{
		m_text = arguments.mtext;
		m_textColor = arguments.mtextColor;
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
		m_font = arguments.mtextInfo;
	}

	uint32_t TextBlockWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		math::float2 shadowOffset = m_textStyle->m_shadowOffset;
		math::float4 shadowColor = m_textStyle->m_shadowColor;

		uint32_t highestLayer = layer;
		if (shadowColor.w != 0.0f)
		{
			//draw shadow
			//highestLayer = std::max(highestLayer, (uint32_t)m_textLayoutCache->OnPaint(paintArgs, allocatedGeometry.getOffsetGeometry(math::float2(shadowOffset.x, shadowOffset.y)), cullingRect, elementList, layer, *m_textStyle));
			//ElementList::addTextElement(elementList, allocatedGeometry.getOffsetGeometry(math::float2(shadowOffset.x, shadowOffset.y)), m_textStyle->m_shadowColor, m_textStyle->m_textInfo, m_text.Get(), layer);
			//++layer;
		}

        math::float4 textColor = m_textColor.Get() ? m_textColor.Get() : m_textStyle->m_textColor;
		//todo:generate text element
		//ElementList::addTextElement(elementList, allocatedGeometry, textColor, m_textStyle->m_textInfo, m_text.Get(), layer);
		highestLayer = std::max(highestLayer, (uint32_t)m_textLayoutCache->OnPaint(paintArgs, allocatedGeometry, cullingRect, elementList, layer, *m_textStyle));
		return highestLayer;
	}
	GuGu::math::float2 TextBlockWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		TextBlockStyle computedTextStyle = getComputedTextStyle();

		return m_textLayoutCache->ComputeFixedSize(
			TextBlockLayout::WidgetDesiredSizeArgs(
				m_text.Get(),
				Padding(0.0f, 0.0f, 0.0f, 0.0f),
				0,
				TextJustify::Type::Left,
				false
			),
			inLayoutScaleMultiplier,
			getComputedTextStyle()
		);

		//return FontCache::getFontCache()->measureText(m_text.Get(), *m_textStyle->m_textInfo, inLayoutScaleMultiplier);
	}
	void TextBlockWidget::setText(const Attribute<GuGuUtf8Str>& newText)
	{
		m_text = newText;
	}
	std::shared_ptr<TextInfo> TextBlockWidget::getFont() const
	{
		return m_font.IsSet() ? m_font.Get() : m_textStyle->m_textInfo;
	}

	GuGuUtf8Str TextBlockWidget::getText() const
	{
		return m_text.Get();
	}

	TextBlockStyle TextBlockWidget::getComputedTextStyle() const
	{
		TextBlockStyle computedStyle = *m_textStyle;
		computedStyle.m_textInfo = getFont();
		computedStyle.m_textColor = m_textColor.Get();

		return computedStyle;
	}
}