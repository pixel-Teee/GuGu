#pragma once

#include "ListView.h"

namespace GuGu {
	template<typename ItemType>
	class TileView : public ListView<ItemType>
	{
	public:
		using NullableItemType = typename ListTypeTraits<ItemType>::NullableType;
		using OnGenerateRow = typename WidgetDelegates<ItemType>::OnGenerateRow;
		using OnSelectionChanged = typename WidgetDelegates<NullableItemType>::OnSelectionChanged;

		struct BuilderArguments : public Arguments<TileView>
		{
			BuilderArguments()
			{
				this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			UI_EVENT(OnGenerateRow, onGenerateTile) //生成一行

			ARGUMENT_VALUE(const std::vector<ItemType>*, ListItemSource)

			ARGUMENT_ATTRIBUTE(float, itemHeight)

			ARGUMENT_ATTRIBUTE(float, itemWidth)

			ARGUMENT_ATTRIBUTE(ListItemAlignment, itemAlignment)

			UI_EVENT(OnSelectionChanged, onSelectionChanged)

			ARGUMENT_ATTRIBUTE(SelectionMode::Type, selectionMode)

			ARGUMENT_VALUE(std::shared_ptr<ScrollBar>, externalScrollbar)

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_ATTRIBUTE(Visibility, scrollbarVisibility)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, style)

			ARGUMENT_VALUE(bool, isFocusable)
		};
	private:
	};
}