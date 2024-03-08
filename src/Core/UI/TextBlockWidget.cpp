#include <pch.h>

#include "TextBlockWidget.h"

#include "FontCache.h"
#include "Style.h"
#include "StyleSet.h"
#include "ElementList.h"

namespace GuGu {
	TextBlockWidget::TextBlockWidget()
	{
		m_textStyle = StyleSet::getStyle()->getStyle<TextBlockStyle>("normalText");
	}
	TextBlockWidget::~TextBlockWidget()
	{
	}
	void TextBlockWidget::init(const BuilderArguments& arguments)
	{
		m_text = arguments.mtext;
	}
	uint32_t TextBlockWidget::GenerateElement(PaintArgs& paintArgs, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		paintArgs.m_allWidgets.push_back(shared_from_this());
		m_geometry = allocatedGeometry;
		m_layer = layer;
		math::float2 shadowOffset = m_textStyle->m_shadowOffset;
		math::float4 shadowColor = m_textStyle->m_shadowColor;

		if (shadowColor.w != 0.0f)
		{
			//draw shadow
			ElementList::addTextElement(elementList, allocatedGeometry.getOffsetGeometry(math::double2(shadowOffset.x, shadowOffset.y)), m_textStyle->m_shadowColor, m_textStyle->m_textInfo, m_text.Get(), layer);
			++layer;
		}

		//todo:generate text element
		ElementList::addTextElement(elementList, allocatedGeometry, m_textStyle->m_textColor, m_textStyle->m_textInfo, m_text.Get(), layer);

		return layer;
	}
	math::double2 TextBlockWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return FontCache::getFontCache()->measureText(m_text.Get(), *m_textStyle->m_textInfo, inLayoutScaleMultiplier);
	}
	void TextBlockWidget::setText(GuGuUtf8Str& newText)
	{
		m_text = newText;
	}
}