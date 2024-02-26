#pragma once

#include "Widget.h"

namespace GuGu {
	class GuGuUtf8Str;
	class TextInfo;
	class TextBlockStyle;
	class TextBlockWidget : public Widget
	{
	public:
		TextBlockWidget();

		virtual ~TextBlockWidget();

		virtual uint32_t GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		void setText(GuGuUtf8Str& newText);
	private:
		std::shared_ptr<TextBlockStyle> m_textStyle;

		std::shared_ptr<GuGuUtf8Str> m_text;

	};
}