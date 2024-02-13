#include <pch.h>

#include "TextBlockWidget.h"

#include "FontCache.h"

#include "Style.h"

#include "ElementList.h"

namespace GuGu {
	TextBlockWidget::TextBlockWidget()
	{
		m_text = std::make_shared<GuGuUtf8Str>();
		m_text->append(u8"fps");
		m_textInfo = Style::getStyle()->getTextInfo("normalText");
	}
	TextBlockWidget::~TextBlockWidget()
	{
	}
	void TextBlockWidget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		//todo:generate text element
		ElementList::addTextElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_textInfo, *m_text, layer);
	}
	math::double2 TextBlockWidget::ComputeFixedSize()
	{
		return FontCache::getFontCache()->measureText(*m_text, *m_textInfo);
	}
	void TextBlockWidget::setText(GuGuUtf8Str& newText)
	{
		*m_text = newText;
	}
}