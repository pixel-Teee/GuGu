#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "BasicElement.h"
#include "Slot.h"

namespace GuGu {

	using OnCheckStateChanged = std::function<void(CheckBoxState)>;

	class Brush;
	struct CheckBoxStyle;
	class SingleChildSlot;
	class CheckBox : public Widget
	{
	public:
		CheckBox();

		virtual ~CheckBox();

		struct BuilderArguments : public Arguments<CheckBox>
		{
			BuilderArguments() :
				mclickMethod(ButtonClickMethod::Type::MouseUp)
			{

			}

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_MEMBER(CheckBoxStyle, checkBoxStyle)

			ARGUMENT_VALUE(ButtonClickMethod::Type, clickMethod)

			UI_EVENT(OnCheckStateChanged, onCheckStateChanged)

			ARGUMENT_ATTRIBUTE(CheckBoxState, isChecked)
		};

		void init(const BuilderArguments& arguments);

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual Reply OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual bool isInteractable() const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		std::shared_ptr<Brush> onGetBackgroundImage() const;

		std::shared_ptr<Brush> onGetCheckImage() const;

		std::shared_ptr<Brush> getUncheckedHoveredImage() const;

		std::shared_ptr<Brush> getUncheckedImage() const;

		std::shared_ptr<Brush> getCheckedHoveredImage() const;

		std::shared_ptr<Brush> getCheckedImage() const;

		void toggleCheckedState();

		void setIsChecked(Attribute<CheckBoxState> inIsChecked);
	private:
		std::shared_ptr<SingleChildSlot> m_childWidget;

		std::shared_ptr<CheckBoxStyle> m_checkBoxStyle;

		Attribute<CheckBoxState> m_isCheckboxChecked;

		ButtonClickMethod::Type m_clickMethod;

		EnumAsByte<ButtonClickMethod::Type> getClickMethodFromInputType(const PointerEvent& mouseEvent) const;

		bool m_bIsPressed;

		OnCheckStateChanged m_onCheckStateChanged;
	};
}