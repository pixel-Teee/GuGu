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

		struct BuilderArguments : public Arguments<TextBlockWidget>
		{
			BuilderArguments()
			{
				mClip = WidgetClipping::Inherit;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)

            ARGUMENT_ATTRIBUTE(math::float4, textColor)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		void setText(GuGuUtf8Str& newText);
	private:
		std::shared_ptr<TextBlockStyle> m_textStyle;

		Attribute<GuGuUtf8Str> m_text;

        Attribute<math::float4> m_textColor;
	};
}