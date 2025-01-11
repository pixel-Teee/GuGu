#pragma once

#include "IToolTip.h"
#include "CompoundWidget.h"
#include "UIMacros.h"

namespace GuGu {
	class Brush;
	class TextInfo;
	class ToolTip : public CompoundWidget, public IToolTip
	{
	public:
		struct BuilderArguments : public Arguments<ToolTip>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<TextInfo>, font)

			ARGUMENT_ATTRIBUTE(math::float4, colorAndOpacity)

			ARGUMENT_ATTRIBUTE(Padding, textPadding)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, borderImage)

			ARGUMENT_ATTRIBUTE(bool, isInteractive)
		};

		void init(const BuilderArguments& arguments);

		std::shared_ptr<Widget> asWidget() override;

		std::shared_ptr<Widget> getContentWidget() override;

		void setContentWidget(const std::shared_ptr<Widget>& inContentWidget) override;

		bool isEmpty() const override;

		void onOpening() override;

		void onClosed() override;

	private:
		Attribute<GuGuUtf8Str> m_textContent;

		std::weak_ptr<Widget> m_widgetContent;

		std::shared_ptr<Widget> m_toolTipContent;

		Attribute<std::shared_ptr<TextInfo>> m_font;

		Attribute<math::float4> m_colorAndOpacity;

		//文本内容的边距
		Attribute<Padding> m_textMargin;

		Attribute<std::shared_ptr<Brush>> m_borderImage;
	};
}