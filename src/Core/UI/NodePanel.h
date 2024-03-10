#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"
#include "Attribute.h"
#include "BasicElement.h"

namespace GuGu {
	class Brush;
	class NodePanel;
	//namespace NodeZone
	//{
	//	enum Type
	//	{
	//		TopLeft,
	//		TopCenter,
	//		TopRight,
	//
	//		Left,
	//		Center,
	//		Right,
	//
	//		BottomLeft,
	//		BottomCenter,
	//		BottomRight,
	//
	//		Count
	//	};
	//}
	class Node : public Widget
	{
	public:
		class NodeSlot : public Slot<NodeSlot>
		{
		public:
			friend Node;

			struct SlotBuilderArguments : public Slot<NodeSlot>::SlotBuilderArguments
			{
				SlotBuilderArguments(std::shared_ptr<NodeSlot> inSlot)
					: Slot<NodeSlot>::SlotBuilderArguments(inSlot)
				{}

				virtual ~SlotBuilderArguments() {}

				typedef SlotBuilderArguments BuilderArguments;

				ARGUMENT_ATTRIBUTE(Padding, padding)
				ARGUMENT_ATTRIBUTE(math::float2, slotOffset)
				ARGUMENT_ATTRIBUTE(math::float2, slotSize)
				ARGUMENT_ATTRIBUTE(bool, allowScaling)
			};

			void init(const SlotBuilderArguments& builderArguments)
			{
				m_slotPadding = builderArguments.mpadding;
				m_offset = builderArguments.mslotOffset;
				m_size = builderArguments.mslotSize;
				m_bAllowScale = builderArguments.mallowScaling;
			}
		private:
			//the child widget contained in this slot
			Attribute<Padding> m_slotPadding;
			Attribute<math::float2> m_offset;
			Attribute<math::float2> m_size;
			Attribute<bool> m_bAllowScale;
		};

		Node();

		virtual ~Node();

		struct BuilderArguments : public Arguments<Node>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_SLOT(NodeSlot, Slots)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override;

		virtual SlotBase* getSlot(uint32_t index) override;

		virtual uint32_t getSlotsNumber() override;

		virtual math::float2 getPosition() const
		{
			return math::float2(0.0f, 0.0f);
		}
	private:
		Attribute<std::shared_ptr<Brush>> m_borderImage;

		std::vector<std::shared_ptr<NodeSlot>> m_children;
		std::weak_ptr<NodePanel> m_parentPanelPtr;
	};

	class NodePanel : public Widget
	{
	public:
		NodePanel();

		virtual ~NodePanel();

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override;

		virtual SlotBase* getSlot(uint32_t index) override;

		virtual uint32_t getSlotsNumber() override;
	protected:
		std::vector<std::shared_ptr<Slot<Node>>> m_children;
	};
}