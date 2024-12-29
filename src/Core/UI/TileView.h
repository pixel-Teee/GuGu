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
				: mscrollBarStyle(CoreStyle::getStyleSet()->template getStyle<ScrollBarStyle>("ScrollBar"))
				, mitemHeight(128)
				, mitemWidth(128)
				, mitemAlignment(ListItemAlignment::EvenlyDistributed)
				, morientation(Orientation::Vertical)
				, mListItemSource()
				, mselectionMode(SelectionMode::Multi)
			{
				this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			UI_EVENT(OnGenerateRow, onGenerateTile) //生成一行

			UI_EVENT(OnTableViewScrolled, onTileViewScrolled)

			ARGUMENT_VALUE(const std::vector<ItemType>*, ListItemSource)

			ARGUMENT_ATTRIBUTE(float, itemHeight)

			ARGUMENT_ATTRIBUTE(float, itemWidth)

			ARGUMENT_ATTRIBUTE(ListItemAlignment, itemAlignment)

			UI_EVENT(OnSelectionChanged, onSelectionChanged)

			ARGUMENT_ATTRIBUTE(SelectionMode::Type, selectionMode)

			ARGUMENT_VALUE(std::shared_ptr<ScrollBar>, externalScrollbar)

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_ATTRIBUTE(Visibility, scrollbarVisibility)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, scrollBarStyle)

			ARGUMENT_VALUE(bool, isFocusable)
		};

		void init(const BuilderArguments& arguments)
		{
			m_widgetClipping = arguments.mClip;

			//制作table view
			this->m_onGenerateRow = arguments.monGenerateTile;
			this->m_itemsSource = arguments.mListItemSource;
			this->m_selectionMode = arguments.mselectionMode;
			this->m_onSelectionChanged = arguments.monSelectionChanged;

			constructChildren(arguments.mitemWidth, arguments.mitemHeight, arguments.mitemAlignment, std::shared_ptr<HeaderRow>(), arguments.mexternalScrollbar,
				arguments.morientation, arguments.monTileViewScrolled, arguments.mscrollBarStyle);

			//todo:可能这里以后要简化
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
			m_visibilityAttribute = arguments.mVisibility;
		}

		TileView(TableViewMode::Type inListMode = TableViewMode::Tile)
			: ListView<ItemType>(inListMode)
		{
		}

		virtual ReGenerateResults reGenerateItems(const WidgetGeometry& myGeometry) override
		{
			auto doubleFractional = [](double value)->double
			{
				return value - std::trunc(value);//trunc 获取 value 的整数部分，函数返回小数部分
			};

			//从我们的panel清除所有的items
			this->clearWidgets();

			const std::vector<ItemType>* sourceItems = this->m_itemsSource;
			if (sourceItems && sourceItems->size() > 0)
			{
				TableViewDimensions tileDimensions = getTileDimensions();
				TableViewDimensions allottedDimensions(this->m_orientation, myGeometry.getLocalSize());

				const int32_t numItems = sourceItems->size();
				const int32_t numItemsPerLine = getNumItemsPerLine();

				const int32_t numItemsPaddedToFillLastLine = (numItems % numItemsPerLine != 0)
					? numItems + numItemsPerLine - numItems % numItemsPerLine
					: numItems;

				const double linesPerScreen = allottedDimensions.m_scrollAxis / tileDimensions.m_scrollAxis;
				const double endOfListOffset = numItemsPaddedToFillLastLine - numItemsPerLine * linesPerScreen;
				const double clampedScrollOffset = clamp(this->m_currentScrollOffset, 0.0, endOfListOffset);
				const float layoutScaleMultiplier = myGeometry.getAccumulateLayoutTransform().m_linear[0][0];//scale

				TableViewDimensions dimensionsUsedSoFar(this->m_orientation);

				int32_t startIndex = std::max(0, int32_t(std::floor(clampedScrollOffset / numItemsPerLine)) * numItemsPerLine);

				this->m_widgetGenerator.onBeginGenerationPass();

				//真正地生成控件
				bool bIsAtEndOfList = false;
				bool bHasFilledAvailableArea = false;
				bool bNewLine = true;
				bool bFirstLine = true;
				double numLinesShownOnScreen = 0;
				for (int32_t itemIndex = startIndex; !bHasFilledAvailableArea && itemIndex < numItems; ++itemIndex)
				{
					const ItemType& curItem = (*sourceItems)[itemIndex];

					if (bNewLine)
					{
						bNewLine = false;

						float lineFraction = 1.0f;
						if (bFirstLine)
						{
							bFirstLine = false;
							lineFraction -= doubleFractional(clampedScrollOffset / numItemsPerLine);
						}

						dimensionsUsedSoFar.m_scrollAxis += tileDimensions.m_scrollAxis * lineFraction;

						if (dimensionsUsedSoFar.m_scrollAxis > allottedDimensions.m_scrollAxis)
						{
							numLinesShownOnScreen += std::max(1.0f - ((dimensionsUsedSoFar.m_scrollAxis - allottedDimensions.m_scrollAxis) / tileDimensions.m_scrollAxis), 0.0f);
						}
						else
						{
							numLinesShownOnScreen += lineFraction;
						}
					}

					ListView<ItemType>::generateWidgetForItem(curItem, itemIndex, startIndex, layoutScaleMultiplier);

					dimensionsUsedSoFar.m_lineAxis += tileDimensions.m_lineAxis;

					bIsAtEndOfList = itemIndex >= numItems - 1;

					if (dimensionsUsedSoFar.m_lineAxis + tileDimensions.m_lineAxis > allottedDimensions.m_lineAxis)
					{
						dimensionsUsedSoFar.m_lineAxis = 0;
						bNewLine = true;
					}

					if (bIsAtEndOfList || bNewLine)
					{
						const float floatPrecisionOffset = 0.001f;
						bHasFilledAvailableArea = dimensionsUsedSoFar.m_scrollAxis > allottedDimensions.m_scrollAxis + floatPrecisionOffset;
					}
				}

				this->m_widgetGenerator.onEndGenerationPass();

				const float totalGeneratedLineAxisSize = std::ceil(numLinesShownOnScreen) * tileDimensions.m_scrollAxis;
				return TableViewBase::ReGenerateResults(clampedScrollOffset, totalGeneratedLineAxisSize, numLinesShownOnScreen, bIsAtEndOfList && !bHasFilledAvailableArea);
			}	

			return TableViewBase::ReGenerateResults(0, 0, 0, false);
		}

		virtual int32_t getNumItemsBeingObserved() const override
		{
			const int32_t numItemsBeingObserved = this->m_itemsSource == nullptr ? 0 : this->m_itemsSource->size();
			const int32_t numItemsPerLine = getNumItemsPerLine();

			int32_t numEmptySpacesAtEnd = 0;
			if (numItemsPerLine)
			{
				numEmptySpacesAtEnd = numItemsPerLine - (numItemsBeingObserved % numItemsPerLine);
				if (numEmptySpacesAtEnd >= numItemsPerLine)
				{
					numEmptySpacesAtEnd = 0;
				}
			}

			return numItemsBeingObserved + numEmptySpacesAtEnd;
		}

	protected:
		TableViewDimensions getTileDimensions() const
		{
			return TableViewDimensions(this->m_orientation, this->getItemWidth(), this->getItemHeight());
		}
		
		virtual int32_t getNumItemsPerLine() const override
		{
			TableViewDimensions panelDimensions(this->m_orientation, this->m_panelGeometryLastTick.getLocalSize());
			TableViewDimensions tileDimensions = getTileDimensions();

			const int32_t numItemsPerLine = tileDimensions.m_lineAxis > 0 ? (int32_t)(std::floor(panelDimensions.m_lineAxis / tileDimensions.m_lineAxis)) : 1;
			return std::max(1, numItemsPerLine);
		}
	};
}