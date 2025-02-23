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

			OverlaySlot(int32_t zOrder)
				: Slot<OverlaySlot>(HorizontalAlignment::Stretch, VerticalAlignment::Stretch, Padding(0.0f, 0.0f, 0.0f, 0.0f))
				, m_zOrder(zOrder)
			{}

			virtual ~OverlaySlot() {}

			void init(std::shared_ptr<Widget> inParentWidget,const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);
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

		//scoped arguments
		template<typename SlotType>
		struct ScopedWidgetSlotArguments : public SlotType::SlotBuilderArguments
		{
			ScopedWidgetSlotArguments(std::shared_ptr<SlotType> inSlot, std::vector<std::shared_ptr<SlotType>>& inChildren, int32_t index,
				std::shared_ptr<Widget> inParentWidget)
				: SlotType::SlotBuilderArguments(inSlot)
				, m_childrens(inChildren)
				, m_index(index)
				, m_parentWidget(inParentWidget)
			{}

			virtual ~ScopedWidgetSlotArguments()
			{
				if (m_slot != nullptr)
				{
					if (m_index == -1)
					{
						//std::shared_ptr<SlotType> boxSlot = std::make_shared<SlotType>();
						//SlotType::SlotBuilderArguments slotArguments(boxSlot);
						m_slot->init(m_parentWidget, *this);
						m_childrens.push_back(std::static_pointer_cast<SlotType>(std::static_pointer_cast<SlotBase>(m_slot)));
					}
					else
					{
						//std::shared_ptr<SlotType> boxSlot = std::make_shared<SlotType>();
						//SlotType::SlotBuilderArguments slotArguments(boxSlot);
						m_slot->init(m_parentWidget, *this);
						m_childrens.insert(m_childrens.begin() + m_index, std::static_pointer_cast<SlotType>(std::static_pointer_cast<SlotBase>(m_slot)));
					}
				}
			}

		private:
			std::vector<std::shared_ptr<SlotType>>& m_childrens;

			std::shared_ptr<Widget> m_parentWidget;

			int32_t m_index;
		};

		Overlay::ScopedWidgetSlotArguments<OverlaySlot> addSlot(int32_t zOrder = -1);

		void removeSlot(int32_t zOrder = -1);

		bool removeSlot(std::shared_ptr<Widget> widget);
	private:
		std::vector<std::shared_ptr<OverlaySlot>> m_childrens;
	};
}