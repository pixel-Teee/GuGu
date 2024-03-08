#pragma once

#include <Core/GuGuUtf8Str.h>

#include "Widget.h"
#include "Attribute.h"
#include "UIMacros.h"

#include <functional>

namespace GuGu {
	class GuGuUtf8Str;
	class TextInfo;
	class TextBlockStyle;
	class TextBlockWidget : public Widget
	{
	public:
		TextBlockWidget();

		virtual ~TextBlockWidget();

		struct BuilderArguments
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t GenerateElement(PaintArgs& paintArgs, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		void setText(GuGuUtf8Str& newText);
	private:
		std::shared_ptr<TextBlockStyle> m_textStyle;

		Attribute<GuGuUtf8Str> m_text;
	};
}