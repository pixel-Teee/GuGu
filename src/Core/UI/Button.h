#pragma once

#include "Border.h"
#include "Style.h"
#include "Reply.h"

namespace GuGu {
	class Button : public Border
	{
	public:
		Button();

		virtual ~Button();

		struct BuilderArguments
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_VALUE(VerticalAlignment, verticalAlignment)

			ARGUMENT_VALUE(HorizontalAlignment, horizontalAlignment)

			ARGUMENT_ATTRIBUTE(Padding, padding)

			ARGUMENT_MEMBER(ButtonStyle, buttonSyle)

			UI_EVENT(OnClicked, Clicked)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

		//virtual Reply onMouseButtonDown(const WidgetGeometry& geometry);
	private:
		std::shared_ptr<ButtonStyle> m_buttonStyle;

		Attribute<Padding> m_contentPadding;

		OnClicked m_clicked;
	};
}