#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "BasicElement.h"
#include "Slot.h"

namespace GuGu {
	class Brush;
	class CheckBoxStyle;
	class SingleChildSlot;
	class CheckBox : public Widget
	{
	public:
		CheckBox();

		virtual ~CheckBox();

		struct BuilderArguments : public Arguments<CheckBox>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual Reply OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		std::shared_ptr<Brush> onGetBackgroundImage() const;

		std::shared_ptr<Brush> onGetCheckImage() const;

	private:
		std::shared_ptr<SingleChildSlot> m_childWidget;

		std::shared_ptr<CheckBoxStyle> m_checkBoxStyle;

		Attribute<CheckBoxState> m_isCheckboxChecked;

		bool m_bIsPressed;
	};
}