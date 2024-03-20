#include <pch.h>

#include "TextBlockWidget.h"

#include "FontCache.h"
#include "Style.h"
#include "StyleSet.h"
#include "ElementList.h"
#include "PlainTextLayoutMarshaller.h"

namespace GuGu {
	TextBlockWidget::TextBlockWidget()
	{
		m_textStyle = StyleSet::getStyle()->getStyle<TextBlockStyle>("normalText");

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
	}

	uint32_t TextBlockWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		math::float2 shadowOffset = m_textStyle->m_shadowOffset;
		math::float4 shadowColor = m_textStyle->m_shadowColor;

		if (shadowColor.w != 0.0f)
		{
			//draw shadow
			ElementList::addTextElement(elementList, allocatedGeometry.getOffsetGeometry(math::float2(shadowOffset.x, shadowOffset.y)), m_textStyle->m_shadowColor, m_textStyle->m_textInfo, m_text.Get(), layer);
			++layer;
		}

        math::float4 textColor = m_textColor.Get() ? m_textColor.Get() : m_textStyle->m_textColor;
		//todo:generate text element
		ElementList::addTextElement(elementList, allocatedGeometry, textColor, m_textStyle->m_textInfo, m_text.Get(), layer);

		return layer;
	}
	GuGu::math::float2 TextBlockWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		//m_textLayoutCache->ComputeFixedSize(
		//	TextBlockLayout::WidgetDesiredSizeArgs(
		//		m_text.Get(),
		//		Padding(0.0f, 0.0f, 0.0f, 0.0f),
		//		0,
		//		TextJustify::Type::Left,
		//		false
		//	),
		//	inLayoutScaleMultiplier,
		//	*m_textStyle
		//);

		return FontCache::getFontCache()->measureText(m_text.Get(), *m_textStyle->m_textInfo, inLayoutScaleMultiplier);
	}
	void TextBlockWidget::setText(GuGuUtf8Str& newText)
	{
		m_text = newText;
	}
}