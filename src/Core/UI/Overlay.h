#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"


namespace GuGu {
	class Overlay : public Widget
	{
	public:
		Overlay();

		virtual ~Overlay();

		class OverlaySlot : public Slot<OverlaySlot>
		{
		public:
			OverlaySlot() {}

			virtual ~OverlaySlot() {}

			void init(const SlotBuilderArguments& builderArguments)
			{
				
			}

			struct SlotBuilderArguments : public Slot<OverlaySlot>::SlotBuilderArguments
			{
				//using BoxPanel::BoxSlot::SlotBuilderArguments;
				SlotBuilderArguments(std::shared_ptr<OverlaySlot> inSlot)
					: Slot<OverlaySlot>::SlotBuilderArguments(inSlot)
				{}

				virtual ~SlotBuilderArguments() {}
			};
		private:
		};

		struct BuilderArguments : public Arguments<Overlay>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;
		
			ARGUMENT_SLOT(OverlaySlot, Slots)
		};
		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override;

		virtual SlotBase* getSlot(uint32_t index) override;

		virtual uint32_t getSlotsNumber() override;

		static OverlaySlot::SlotBuilderArguments Slot();
	private:
		std::vector<std::shared_ptr<OverlaySlot>> m_childrens;
	};
}