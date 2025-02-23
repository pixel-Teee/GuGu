#pragma once

#include "Widget.h"
#include "Slot.h"
#include "UIMacros.h"
#include "BasicElement.h"
#include "WidgetDelegates.h"
#include "StyleSet.h"
#include "CoreStyle.h"

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
				: Slot<SplitterSlot>(HorizontalAlignment::Stretch, VerticalAlignment::Stretch, Padding(0.0f, 0.0f, 0.0f, 0.0f))
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

			struct SlotBuilderArguments : public Slot<SplitterSlot>::SlotBuilderArguments
			{
				SlotBuilderArguments(std::shared_ptr<SplitterSlot> inSlot)
					: Slot<SplitterSlot>::SlotBuilderArguments(inSlot) {}
				virtual ~SlotBuilderArguments() {};

				SlotBuilderArguments& value(const Attribute<float>& inValue)
				{
					m_sizeValue = inValue;
					return Me();
				}

				SlotBuilderArguments& onSlotResized(const OnSlotResized& inHandler)
				{
					m_onSlotResizedHandler = inHandler;
					return Me();
				}

				SlotBuilderArguments& sizeRule(const Attribute<SizeRule>& inSizeRule)
				{
					m_sizingRule = inSizeRule;
					return Me();
				}

				Attribute<SizeRule> m_sizingRule;
				Attribute<float> m_sizeValue;
				OnSlotResized m_onSlotResizedHandler;
			};

			void init(std::shared_ptr<Widget> inParentWidget, SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);

				m_sizeValue = builderArguments.m_sizeValue;
				m_sizingRule = builderArguments.m_sizingRule;
				m_onSlotResizedHandler = builderArguments.m_onSlotResizedHandler;
			}

			Attribute<SizeRule> m_sizingRule;
			Attribute<float> m_sizeValue;
			OnSlotResized m_onSlotResizedHandler;
		};

		struct BuilderArguments : public Arguments<Splitter>
		{
			BuilderArguments() 
				: msplitterStyle(CoreStyle::getStyleSet()->getStyle<SplitterStyle>(u8"splitter"))
				, morientation(Orientation::Horizontal)
				, mresizeMode(SplitterResizeMode::FixedPosition)
				, mphysicalSplitterHandleSize(5.0f)
				, mhitDetectionSplitterHandleSize(5.0f)
				, mminimumSlotHeight(20.0f)
				, monSplitterFinishedResizing()
			{
				mVisibility = Visibility::Visible;
			}

			~BuilderArguments() = default;

			ARGUMENT_SLOT(SplitterSlot, Slots)

			//style
			ARGUMENT_MEMBER(SplitterStyle, splitterStyle)

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_VALUE(SplitterResizeMode::Type, resizeMode)

			ARGUMENT_VALUE(float, physicalSplitterHandleSize) //拖动柄的大小

			ARGUMENT_VALUE(float, hitDetectionSplitterHandleSize)

			ARGUMENT_VALUE(float, minimumSlotHeight)

			UI_EVENT(SimpleDelegate, onSplitterFinishedResizing)
		};

		void init(const BuilderArguments& arguments);

		static SplitterSlot::SlotBuilderArguments Slot();

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

		ScopedWidgetSlotArguments<SplitterSlot> addSlot(int32_t atIndex = -1);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		std::vector<WidgetGeometry> arrangedChildrenForLayout(const WidgetGeometry& allottedGeometry) const;

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		void handleResizingByMousePosition(Orientation orientation, const float physicalSplitterHandleSize, const SplitterResizeMode::Type resizeMode, int32_t draggedHandle, const math::float2& localMousePos, std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children, const std::vector<WidgetGeometry>& childGeometries);

		void handleResizingDelta(Orientation orientation, const float physicalSplitterHandleSize, const SplitterResizeMode::Type resizeMode, int32_t draggedHandle, float delta, std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children, const std::vector<WidgetGeometry>& childGeometries);
		void handleResizingBySize(Orientation orientation, const float physicalSplitterHandleSize, const SplitterResizeMode::Type resizeMode, int32_t draggedHandle, const math::float2& desiredSize, std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children, const std::vector<WidgetGeometry>& childGeometries);

		static int32_t findResizeableSlotBeforeHandle(int32_t draggedHandle, const std::vector<std::shared_ptr<SplitterSlot>>& children);
		void findAllResizeableSlotsAfterHandle(int32_t draggedHandle, const std::vector<std::shared_ptr<SplitterSlot>>& children, std::vector<int32_t>& outSlotIndices);
		int32_t findResizeableSlotAfterHandle(int32_t draggedHandle, const std::vector<std::shared_ptr<SplitterSlot>>& children);

		float clampChild(float proposedSize);

		template<Orientation orientation>
		int32_t getHandleBeingResizedFromMousePosition(float inPhysicalSplitterHandleSize, float hitDetectionSplitterHandleSize, math::float2 localMousePos, const std::vector<WidgetGeometry>& childGeometries);
	private:

		Orientation m_orientation;

		SplitterResizeMode::Type m_resizeMode;

		std::vector<std::shared_ptr<SplitterSlot>> m_childrens;

		float m_physicalSplitterHandleSize;
		float m_hitDetectionSplitterHandleSize;

		int32_t m_hoveredHandleIndex;
		bool m_bIsResizing;

		float m_minSplitterChildLength;

		std::shared_ptr<SplitterStyle> m_style;
	};
}