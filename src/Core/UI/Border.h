#pragma once

#include "Widget.h"
#include "Slot.h"
#include "Brush.h"
#include "Attribute.h"
#include "UIMacros.h"

namespace GuGu {
	class Border : public Widget
	{
	public:
		Border();

		virtual ~Border();

		struct BuilderArguments
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_VALUE(VerticalAlignment, verticalAlignment)
			
			ARGUMENT_VALUE(HorizontalAlignment, horizontalAlignment)
			
			ARGUMENT_VALUE(Padding, padding)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t GenerateElement(PaintArgs& paintArgs, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual void AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override;

		virtual SlotBase* getSlot(uint32_t index) override;

		virtual uint32_t getSlotsNumber() override;
	protected:
		Attribute<std::shared_ptr<Brush>> m_imageBursh;

		std::shared_ptr<SingleChildSlot> m_childWidget;
	};
}