#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

#include "TableViewBase.h"//ListItemAlignment depends on it

namespace GuGu {
	//排列儿子控件在一个垂直的列表，之间没有间隔
	//items 在这个 panel ，有一个统一的高度
	//也支持垂直地偏移它们的item
	class ListPanel : public Widget
	{
	public:
		ListPanel();

		virtual ~ListPanel();

		class PaneSlot : public Slot<PaneSlot>
		{
		public:
			PaneSlot() {}

			virtual ~PaneSlot() = default;
		};

		struct BuilderArguments : public Arguments<ListPanel>
		{
			BuilderArguments()
				: mItemWidth(0)
				, mItemHeight(16)
				, mNumDesiredItems(0)
				, mItemAlignment(ListItemAlignment::EvenlyDistributed)
				, mListOrientation(Orientation::Horizontal)
			{
				//mVisibility = Visibility::SelfHitTestInvisible;
				mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			//ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)
			ARGUMENT_SLOT(PaneSlot, Slots)

			ARGUMENT_ATTRIBUTE(float, ItemWidth)

			ARGUMENT_ATTRIBUTE(float, ItemHeight)

			ARGUMENT_ATTRIBUTE(int32_t, NumDesiredItems)

			ARGUMENT_VALUE(ListItemAlignment, ItemAlignment)

			ARGUMENT_VALUE(Orientation, ListOrientation)
		};

		void init(const BuilderArguments& arguments);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

		virtual SlotBase* getSlot(uint32_t index)  const override;

		virtual uint32_t getSlotsNumber() const override;

		TableViewDimensions getDesiredItemDimensions() const const;

		//返回应用到每个 tile item 的水平 padding
		float getItemPadding(const WidgetGeometry& allottedGeometry, const ListItemAlignment listItemAlignment) const;

		TableViewDimensions getItemSize(const WidgetGeometry& allottedGeometry) const;
		//返回 uniform item width ，当安排 children 的时候
		TableViewDimensions getItemSize(const WidgetGeometry& allottedGeometry, const ListItemAlignment listItemAlignment) const;

		float getLinePadding(const WidgetGeometry& allottedGeometry, const int32_t lineStartIndex) const;

		static ListPanel::PaneSlot::SlotBuilderArguments Slot();

	protected:

		bool shouldArrangeAsTiles() const;

	protected:
		std::vector<std::shared_ptr<PaneSlot>> m_childrens;

		//统一的 item 宽度，用来排列儿子，只对 tile views 有关
		Attribute<float> m_itemWidth;

		//统一的 item 高度，用来排列儿子
		Attribute<float> m_itemHeight;

		//总共数量的items想去显示
		Attribute<int32_t> m_numDesiredItems;

		//从 list top 顶部开始的view area 的 offset ，item heights
		float m_firstLineScrollOffset = 0.0f;

		//移动的数量
		float m_overScrollAmount = 0.0f;

		//水平如何对齐，只对 tile view 有关
		Attribute<ListItemAlignment> m_itemAlignment;

		//list 的总的方向，对于布局和滑动，只对 tile view 有关
		Orientation m_orientation;

		//这个 widget 希望的一行的数量，正交于scroll axis ，只对 tile view 有关
		int32_t m_preferredNumLines = 1;
	};
}