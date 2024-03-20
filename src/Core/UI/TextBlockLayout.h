#pragma once

#include "Style.h"//TextBlockStyle depends this header file
#include "GuGuTextLayout.h"

namespace GuGu {
	class Widget;
	//class TextLayout;
	class ITextLayoutMarshaller;

	struct PaintArgs;
	class WidgetGeometry;
	class ElementList;
	class Style;
	class TextBlockLayout
	{
	public:
		TextBlockLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle, std::shared_ptr<ITextLayoutMarshaller> inMarshaller);

		virtual ~TextBlockLayout();

		struct WidgetDesiredSizeArgs
		{
			WidgetDesiredSizeArgs(
				const GuGuUtf8Str& inText,
				const Padding& inMargin,
				const float inWrapTextAt,
				const TextJustify::Type inJustification,
				const bool inAutoWrapText
			) :
				text(inText)
				, margin(inMargin)
				, wrapTextAt(inWrapTextAt)
				, justification(inJustification)
				, autoWrapText(inAutoWrapText)
			{ }
			const GuGuUtf8Str text = GuGuUtf8Str();
			const Padding margin;
			const float wrapTextAt = 0.0f;
			const TextJustify::Type justification;
			const bool autoWrapText = false;
		};

		int32_t OnPaint(const PaintArgs& inPaintArgs, const WidgetGeometry& inAllottedGeometry,
			const math::box2& cullingRect, ElementList& outDrawElement, int32_t layerId, const Style& inWidgetStyle);

		math::float2 ComputeFixedSize(const WidgetDesiredSizeArgs& inWidgetArgs, const float inScale, const TextBlockStyle& inTextStyle);

		float calculateWrappingWidth() const;

	private:
		void updateTextLayout(const GuGuUtf8Str& inText);

		std::shared_ptr<GuGuTextLayout> m_textLayout;

		std::shared_ptr<ITextLayoutMarshaller> m_marshaller;

		math::float2 m_cachedSize;

		float m_cachedWrapTextAt;

		bool m_bCachedAutoWrapText;
	};
}