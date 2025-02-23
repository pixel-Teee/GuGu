#pragma once

#include <Core/GuGuUtf8Str.h>

#include "Widget.h"
#include "Attribute.h"
#include "UIMacros.h"
#include "TextBlockLayout.h"

#include <functional>

namespace GuGu {
	class GuGuUtf8Str;
	class TextInfo;
	struct TextBlockStyle;
	class TextBlockWidget : public Widget
	{
	public:
		TextBlockWidget();

		virtual ~TextBlockWidget();

		struct BuilderArguments : public Arguments<TextBlockWidget>
		{
			BuilderArguments()
			{
				mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)

            ARGUMENT_ATTRIBUTE(math::float4, textColor)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<TextInfo>, textInfo)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		void setText(GuGuUtf8Str& newText);

		std::shared_ptr<TextInfo> getFont() const;
	private:
		//获取计算的 text style ，用于 text marshaller
		TextBlockStyle getComputedTextStyle() const;

		std::shared_ptr<TextBlockStyle> m_textStyle;

		Attribute<GuGuUtf8Str> m_text;

        Attribute<math::float4> m_textColor;

		std::unique_ptr<TextBlockLayout> m_textLayoutCache;

		Attribute<std::shared_ptr<TextInfo>> m_font;
	};
}