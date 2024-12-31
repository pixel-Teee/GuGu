#pragma once

#include "Widget.h"
#include "Slot.h"
#include "UIMacros.h"
#include "BasicElement.h"
#include "WidgetDelegates.h"

namespace GuGu {
	namespace SplitterResizeMode
	{
		enum Type
		{
			FixedPosition,
			FixedSize,
			Fill
		};
	}
	class Splitter : public Widget
	{
	public:
		enum SizeRule
		{
			SizeToContent,

			FractionOfParent
		};
		Splitter();

		virtual ~Splitter();

		using OnSlotResized = std::function<void(float)>;

		class SplitterSlot : public Slot<SplitterSlot>
		{
		public:
			SplitterSlot()
				: Slot<SplitterSlot>()
				, m_sizingRule(FractionOfParent)
				, m_sizeValue(1)
			{}

			SplitterSlot& value(const Attribute<float>& inValue)
			{
				m_sizeValue = inValue;
				return *this;
			}

			SplitterSlot& onSlotResized(const OnSlotResized& inHandler)
			{
				m_onSlotResizedHandler = inHandler;
				return *this;
			}

			SplitterSlot& sizeRule(const Attribute<SizeRule>& inSizeRule)
			{
				m_sizingRule = inSizeRule;
				return *this;
			}

			virtual ~SplitterSlot() {}

			void init(std::shared_ptr<Widget> inParentWidget)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);
			}

			Attribute<SizeRule> m_sizingRule;
			Attribute<float> m_sizeValue;
			OnSlotResized m_onSlotResizedHandler;
		};

		struct BuilderArguments : public Arguments<Splitter>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_SLOT(SplitterSlot, Slots)

			//style

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_VALUE(SplitterResizeMode::Type, resizeMode)

			ARGUMENT_VALUE(float, physicalSplitterHandleSize) //拖动柄的大小

			ARGUMENT_VALUE(float, hitDetectionSplitterHandleSize)

			ARGUMENT_VALUE(float, minimumSlotHeight)

			UI_EVENT(SimpleDelegate, onSplitterFinishedResizing)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		std::vector<WidgetGeometry> arrangedChildrenForLayout(const WidgetGeometry& allottedGeometry) const;
	private:

		Orientation m_orientation;

		SplitterResizeMode::Type m_resizeMode;

		std::vector<std::shared_ptr<SplitterSlot>> m_childrens;

		float m_physicalSplitterHandleSize;
		float m_hitDetectionSplitterHandleSize;

		int32_t m_hoveredHandleIndex;
		bool m_bIsResizing;

		float m_minSplitterChildLength;
	};
}