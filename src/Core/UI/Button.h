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

		struct BuilderArguments : public Arguments<Button>
		{
			BuilderArguments()
				:misFocusable(true)
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_VALUE(VerticalAlignment, verticalAlignment)

			ARGUMENT_VALUE(HorizontalAlignment, horizontalAlignment)

			ARGUMENT_ATTRIBUTE(Padding, contentPadding)

			ARGUMENT_MEMBER(ButtonStyle, buttonSyle)

			ARGUMENT_VALUE(bool, isFocusable)

			UI_EVENT(OnClicked, Clicked)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual Reply OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual void OnMouseEnter(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual void OnMouseLeave(const PointerEvent& inMouseEvent) override;

		virtual bool supportsKeyboardFocus() const override;

		virtual bool isInteractable() const override;

		void executeHoverStateChanged();

		void updateBorderImage();
	private:
		void Press();

		void Release();

		std::shared_ptr<ButtonStyle> m_buttonStyle;

		Attribute<Padding> m_contentPadding;

		OnClicked m_clicked;

		bool m_bIsPressed;

		bool m_bIsFocusable;
	};
}