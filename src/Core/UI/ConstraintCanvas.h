#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "Slot.h"

namespace GuGu {
	class ConstraintCanvas : public Widget
	{
	public:
		ConstraintCanvas();

		virtual ~ConstraintCanvas();

		//------pre-child info------
		class CanvasSlot : public Slot<CanvasSlot>
		{
		public:
			CanvasSlot()
				: m_offsetAttr(Padding(0, 0, 1, 1))
				, m_anchorsAttr(Anchors(0.0f, 0.0f))
				, m_alignmentAttr(math::float2(0.5f, 0.5f))
				, m_bAutoSizeAttr(false)
				, m_zOrder(0)
			{
				
			}

			virtual ~CanvasSlot() = default;

			struct SlotBuilderArguments : public Slot<CanvasSlot>::SlotBuilderArguments
			{
				SlotBuilderArguments(std::shared_ptr<CanvasSlot> inSlot)
					: Slot<CanvasSlot>::SlotBuilderArguments(inSlot)
				{}

				Attribute<Padding> m_offsetAttr;
				Attribute<Anchors> m_anchorsAttr;
				Attribute<math::float2> m_alignmentAttr;
				Attribute<bool> m_bAutoSizeAttr;
				Attribute<float> m_zOrder;

				SlotBuilderArguments& offset(Attribute<Padding> inOffset)
				{
					m_offsetAttr = inOffset;
					return Me();
				}

				SlotBuilderArguments& anchors(Attribute<Anchors> inAnchors)
				{
					m_anchorsAttr = inAnchors;
					return Me();
				}

				SlotBuilderArguments& alignment(Attribute<math::float2> inAlignment)
				{
					m_alignmentAttr = inAlignment;
					return Me();
				}

				SlotBuilderArguments& Offset(Attribute<bool> inAutoSize)
				{
					m_bAutoSizeAttr = inAutoSize;
					return Me();
				}

				SlotBuilderArguments& Offset(Attribute<float> inZorder)
				{
					m_zOrder = inZorder;
					return Me();
				}
			};

			void init(std::shared_ptr<Widget> inParentWidget, const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);

				//------construct info------
				m_offsetAttr = builderArguments.m_offsetAttr;
				m_anchorsAttr = builderArguments.m_anchorsAttr;
				m_alignmentAttr = builderArguments.m_alignmentAttr;
				m_bAutoSizeAttr = builderArguments.m_bAutoSizeAttr;
				m_zOrder = builderArguments.m_zOrder;
				//------construct info------
			}

			Attribute<Padding> m_offsetAttr;
			Attribute<Anchors> m_anchorsAttr;
			Attribute<math::float2> m_alignmentAttr;
			Attribute<bool> m_bAutoSizeAttr;
			Attribute<float> m_zOrder;
		};
		//------pre-child info------
		struct BuilderArguments : public Arguments<ConstraintCanvas>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_SLOT(CanvasSlot, Slots)
		};

		void init(const BuilderArguments& arguments);

		static CanvasSlot::SlotBuilderArguments Slot();

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

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
				if (SlotType::SlotBuilderArguments::m_slot != nullptr)
				{
					if (m_index == -1)
					{
						//std::shared_ptr<SlotType> boxSlot = std::make_shared<SlotType>();
						//SlotType::SlotBuilderArguments slotArguments(boxSlot);
						SlotType::SlotBuilderArguments::m_slot->init(m_parentWidget, *this);
						m_childrens.push_back(std::static_pointer_cast<SlotType>(std::static_pointer_cast<SlotBase>(SlotType::SlotBuilderArguments::m_slot)));
					}
					else
					{
						//std::shared_ptr<SlotType> boxSlot = std::make_shared<SlotType>();
						//SlotType::SlotBuilderArguments slotArguments(boxSlot);
						SlotType::SlotBuilderArguments::m_slot->init(m_parentWidget, *this);
						m_childrens.insert(m_childrens.begin() + m_index, std::static_pointer_cast<SlotType>(std::static_pointer_cast<SlotBase>(SlotType::SlotBuilderArguments::m_slot)));
					}
				}
			}
		private:
			std::vector<std::shared_ptr<SlotType>>& m_childrens;

			std::shared_ptr<Widget> m_parentWidget;

			int32_t m_index;
		};

		ScopedWidgetSlotArguments<CanvasSlot> addSlot();
	private:
		using ArrangedChildLayers = std::vector<bool>;

		void arrangeLayeredChildren(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray, ArrangedChildLayers& arrangedChildLayers) const;
	protected:
		std::vector<std::shared_ptr<CanvasSlot>> m_childrens;
	};
}