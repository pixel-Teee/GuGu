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
			OverlaySlot()
				: Slot<OverlaySlot>(HorizontalAlignment::Stretch, VerticalAlignment::Stretch, Padding(0.0f, 0.0f, 0.0f, 0.0f))
				, m_zOrder(0)
			{}

			virtual ~OverlaySlot() {}

			void init(std::shared_ptr<Widget> inParentWidget,const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				//m_childWidget->setParentWidget(inParentWidget);
			}

			struct SlotBuilderArguments : public Slot<OverlaySlot>::SlotBuilderArguments
			{
				//using BoxPanel::BoxSlot::SlotBuilderArguments;
				SlotBuilderArguments(std::shared_ptr<OverlaySlot> inSlot)
					: Slot<OverlaySlot>::SlotBuilderArguments(inSlot)
				{}

				virtual ~SlotBuilderArguments() {}
			};
		
			//大的会绘制在小的上面
			int32_t m_zOrder;
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

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		static OverlaySlot::SlotBuilderArguments Slot();

		OverlaySlot::SlotBuilderArguments addSlot(int32_t zOrder = -1);

		void removeSlot(int32_t zOrder = -1);

		bool removeSlot(std::shared_ptr<Widget> widget);
	private:
		std::vector<std::shared_ptr<OverlaySlot>> m_childrens;
	};
}