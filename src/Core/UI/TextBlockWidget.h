#pragma once

#include "Widget.h"

namespace GuGu {
	class GuGuUtf8Str;
	class TextInfo;
	class TextBlockWidget : public Widget
	{
	public:
		TextBlockWidget();

		virtual ~TextBlockWidget();

		virtual void GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::double2 ComputeFixedSize();

		void setText(GuGuUtf8Str& newText);
	private:
		std::shared_ptr<TextInfo> m_textInfo;

		std::shared_ptr<GuGuUtf8Str> m_text;

	};
}