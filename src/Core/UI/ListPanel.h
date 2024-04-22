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
				mVisibility = Visibility::SelfHitTestInvisible;
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

	};
}