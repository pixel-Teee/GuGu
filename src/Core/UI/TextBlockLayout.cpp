#include <pch.h>

#include "TextBlockLayout.h"
#include "GuGuTextLayout.h"
#include "PlainTextLayoutMarshaller.h"
#include "WidgetGeometry.h"

namespace GuGu {
	TextBlockLayout::TextBlockLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle, std::shared_ptr<ITextLayoutMarshaller> inMarshaller)
		: m_textLayout(std::make_shared<GuGuTextLayout>(inOwner, inDefaultTextStyle))
		, m_marshaller(std::move(inMarshaller))
		, m_cachedSize(math::float2(0.0f, 0.0f))
		, m_cachedWrapTextAt(0)
		, m_bCachedAutoWrapText(false)
	{
	}
	TextBlockLayout::~TextBlockLayout()
	{
	}
	int32_t TextBlockLayout::OnPaint(const PaintArgs& inPaintArgs, const WidgetGeometry& inAllottedGeometry, const math::box2& cullingRect, ElementList& outDrawElement, int32_t layerId, const Style& inWidgetStyle)
	{
		m_cachedSize = inAllottedGeometry.getLocalSize();

		m_textLayout->setWrappingWidth(calculateWrappingWidth());

		return m_textLayout->OnPaint(inPaintArgs, inAllottedGeometry, cullingRect, outDrawElement, layerId, inWidgetStyle);
	}
	math::float2 TextBlockLayout::ComputeFixedSize(const WidgetDesiredSizeArgs& inWidgetArgs, const float inScale, const TextBlockStyle& inTextStyle)
	{
		m_cachedWrapTextAt = inWidgetArgs.wrapTextAt;
		m_bCachedAutoWrapText = inWidgetArgs.autoWrapText;

		//设置 text layout 信息，text layout 是根据文本生成的布局和绘制信息的集合体
		m_textLayout->setScale(inScale);
		m_textLayout->setWrappingWidth(calculateWrappingWidth());
		m_textLayout->setMargin(inWidgetArgs.margin);
		m_textLayout->setJustification(inWidgetArgs.justification);

		m_textLayout->setDefaultTextStyle(inTextStyle);

		updateTextLayout(inWidgetArgs.text);//使用marshaller来填充text layout

		return m_textLayout->getSize();
	}
	float TextBlockLayout::calculateWrappingWidth() const
	{
		float wrappingWidth = m_cachedWrapTextAt;//这个m_cachedSize由OnPaint分配实际布局空间的时候，缓存的文本框的宽度，而m_cachedWrapTextAt则是用户指定的
		if (m_bCachedAutoWrapText && m_cachedSize.x >= 1.0f)//是否使用OnPaint缓存的文本框宽度
		{
			wrappingWidth = (wrappingWidth >= 1.0f) ? std::min(wrappingWidth, m_cachedSize.x) : m_cachedSize.x;
		}
		return std::max(0.0f, wrappingWidth);
	}
	void TextBlockLayout::updateTextLayout(const GuGuUtf8Str& inText)
	{
		m_textLayout->clearLines();
		m_marshaller->setText(inText, *m_textLayout);//通过装配器来填充TextLayout
	}
}