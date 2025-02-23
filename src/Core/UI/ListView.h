#pragma once

#include "TableViewBase.h" //table view base
#include "WidgetDelegates.h"
#include "TableViewTypeTraits.h"
#include "TableRow.h"
#include "TextBlockWidget.h"

#include <unordered_set>

namespace GuGu {
	template<typename ItemType>
	class ListView : public TableViewBase, public ITypedTableView<ItemType>
	{
	public:
		using NullableItemType = typename ListTypeTraits<ItemType>::NullableType;
		using OnGenerateRow = typename WidgetDelegates<ItemType>::OnGenerateRow;
		using OnSelectionChanged = typename WidgetDelegates<NullableItemType>::OnSelectionChanged;
		using OnMouseButtonDoubleClick = typename WidgetDelegates<ItemType>::OnMouseButtonDoubleClick;
		using ItemSet = std::unordered_set<ItemType>;

		ListView(TableViewMode::Type inListMode = TableViewMode::List)
			: TableViewBase(inListMode)
			, m_itemsSource(nullptr)
			, m_isFocusable(true)
			, m_widgetGenerator(this)
		{
			this->m_widgetClipping = WidgetClipping::ClipToBounds;
		}

		virtual ~ListView() {}

		struct BuilderArguments : public Arguments<ListView>
		{
			BuilderArguments()
				: mstyle(CoreStyle::getStyleSet()->template getStyle<ScrollBarStyle>("ScrollBar"))
				, mitemHeight(16)
				, morientation(Orientation::Vertical)
				, mListItemSource()
				, mselectionMode(SelectionMode::Single)
			{
				this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, style)

			ARGUMENT_ATTRIBUTE(SelectionMode::Type, selectionMode)

			UI_EVENT(OnGenerateRow, onGenerateRow)

			UI_EVENT(OnSelectionChanged, onSelectionChanged)

			UI_EVENT(OnTableViewScrolled, onListViewScrolled)

			UI_EVENT(OnMouseButtonDoubleClick, onMouseButtonDoubleClick)

			ARGUMENT_ATTRIBUTE(float, itemHeight)

			ARGUMENT_VALUE(std::shared_ptr<HeaderRow>, headerRow)

			ARGUMENT_VALUE(std::shared_ptr<ScrollBar>, externalScrollbar) //可以控制 list view 的外部的 scroll bar

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_VALUE(const std::vector<ItemType>*, ListItemSource)

		};

		void init(const BuilderArguments& arguments)
		{
			m_widgetClipping = arguments.mClip;
			//制作table view
			this->m_onGenerateRow = arguments.monGenerateRow;
			this->m_itemsSource = arguments.mListItemSource;
			this->m_selectionMode = arguments.mselectionMode;
			this->m_onSelectionChanged = arguments.monSelectionChanged;
			this->m_onDoubleClick = arguments.monMouseButtonDoubleClick;

			constructChildren(0, arguments.mitemHeight, ListItemAlignment::LeftAligned, arguments.mheaderRow, arguments.mexternalScrollbar,
				arguments.morientation, arguments.monListViewScrolled, arguments.mstyle);
			
			//todo:可能这里以后要简化
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
			m_visibilityAttribute = arguments.mVisibility;
		}

		virtual int32_t getNumItemsBeingObserved() const override
		{
			return m_itemsSource == nullptr ? 0 : m_itemsSource->size();
		}

		virtual bool supportsKeyboardFocus() const override
		{
			return m_isFocusable.Get();
		}

		//是否这里有一个待处理的请求去滑动一个 item 到 view 里面
		virtual ScrollIntoViewResult scrollIntoView(const WidgetGeometry& listViewGeometry) override
		{
			if (m_itemsSource && ListTypeTraits<ItemType>::isPtrValid(m_itemToScrollIntoView))
			{
				auto it = std::find(m_itemsSource->begin(), m_itemsSource->end(), ListTypeTraits<ItemType>::nullableItemTypeConverToItemType(m_itemToScrollIntoView));

				if (it != m_itemsSource->end())
				{
					const int32_t indexOfItem = it - m_itemsSource->begin();

					double numLiveWidgets = getNumLiveWidgets();//可以塞入屏幕的 widgets ，也就是 list view 可以显示的 widgets 个数
					if (numLiveWidgets == 0 && isPendingRefresh())
					{
						//m_lastGenerateResults 是上一次生成的过程的信息
						numLiveWidgets = m_lastGenerateResults.m_exactNumLinesOnScreen;
						
						if (numLiveWidgets == 0)
						{
							return ScrollIntoViewResult::Deferred;
						}
					}

					//todo:add end inertial scrolling

					const int32_t numFullEntriesInView = (int32_t)(std::floor(m_currentScrollOffset + numLiveWidgets)
						- std::ceil(m_currentScrollOffset));

					//只滚动 item 到 view 里面，如果它本身不在可见的范围内
					const double minDisplayedIndex = m_bNavigateOnScrollIntoView ? std::floor(m_currentScrollOffset)
						: std::ceil(m_currentScrollOffset);
					const double maxDisplayedIndex = m_bNavigateOnScrollIntoView ? std::ceil(m_currentScrollOffset + numFullEntriesInView) :
						std::floor(m_currentScrollOffset + numFullEntriesInView);

					//需要滚进去
					if (indexOfItem < minDisplayedIndex || indexOfItem > maxDisplayedIndex)
					{
						//将 list view 的顶部滚动到item
						double newScrollOffset = indexOfItem;

						//将 list view 的这个 item 居中
						newScrollOffset -= (numLiveWidgets / 2.0f);

						//限制 list 的 top 和 bottom 的 offset
						const double maxScrollOffset = std::max(0.0, (double)m_itemsSource->size() - numLiveWidgets);
						newScrollOffset = std::clamp(newScrollOffset, 0.0, maxScrollOffset);

						setScrollOffset(newScrollOffset);
					}
					else if (m_bNavigateOnScrollIntoView)
					{
						//todo ：完成这里的逻辑
					}

					requestLayoutRefresh();

					m_itemToNotifyWhenInView = m_itemToScrollIntoView;
				}

				ListTypeTraits<ItemType>::resetPtr(m_itemToScrollIntoView);
			}

			if (ListTypeTraits<ItemType>::isPtrValid(m_itemToScrollIntoView))
			{
				//todo : 动画判断
			}
			else
			{
				return ScrollIntoViewResult::Failure;
			}

			return ScrollIntoViewResult::Success;
			
		}

		virtual std::shared_ptr<ITableRow> generateNewWidget(ItemType inItem)
		{
			if (m_onGenerateRow)
			{
				return m_onGenerateRow(inItem, std::static_pointer_cast<TableViewBase>(shared_from_this()));
			}
			else
			{
				std::shared_ptr<TableRow<ItemType>> newListItemWidget = 
				WIDGET_NEW(TableRow<ItemType>, std::static_pointer_cast<TableViewBase>(shared_from_this()))
				.Content
				(
					WIDGET_NEW(TextBlockWidget)
					.text("OnGenerateWidget() not assigned")
				);
				return newListItemWidget;
                //return nullptr;
			}
		}

		float generateWidgetForItem(const ItemType& curItem, int32_t itemIndex, int32_t startIndex, float layoutScaleMultiplier)
		{
			std::shared_ptr<ITableRow> widgetForItem = m_widgetGenerator.getWidgetForItem(curItem);

			if (!widgetForItem)
			{
				widgetForItem = this->generateNewWidget(curItem);//调用用户提供的回调，去生成 widget
			}

			//给 table row 一个在 item 的索引，帮助 偶数/奇数 颜色
			widgetForItem->setIndexInList(itemIndex);

			m_widgetGenerator.onItemSeen(curItem, widgetForItem);//这个 item seen 很重要，负责关联 widgets 和 item 在 widget generator 里面

			const std::shared_ptr<Widget> newlyGeneratedWidget = widgetForItem->asWidget();
			newlyGeneratedWidget->prepass(layoutScaleMultiplier);

			if (itemIndex >= startIndex)
			{
				this->appendWidget(widgetForItem);
			}
			else
			{
				this->insertWidget(widgetForItem);
			}

			const bool bIsVisible = newlyGeneratedWidget->getVisibility().isVisible();
			TableViewDimensions generatedWidgetDimensions(this->m_orientation, bIsVisible ? newlyGeneratedWidget->getFixedSize() : math::float2(0.0f, 0.0f));
			return generatedWidgetDimensions.m_scrollAxis;
		}

		virtual ReGenerateResults reGenerateItems(const WidgetGeometry& myGeometry) override
		{
			auto doubleFractional = [](double value)->double
			{
				return value - std::trunc(value);//trunc 获取 value 的整数部分，函数返回小数部分
			};

			//清除我们的 panel 的所有 items
			this->clearWidgets();

			GenerationPassGuard generationPassGuard(m_widgetGenerator);//这个会调用 widget generator 会清理之前的 widgets

			if (m_itemsSource && m_itemsSource->size() > 0)
			{
				//items 在 view 里面，包含分数 items
				float itemsInView = 0.0f;

				//到目前为止生成的 widgets 的总共长度 (高度对于垂直列表，宽度对于水平)
				float lengthGeneratedSoFar = 0.0f;

				//生成的 widgets 的长度，在 view 的边界内
				float viewLengthUsedSoFar = 0.0f;

				//我们开始生成的 item index ，基于我们已经滚动了多远
				int32_t startIndex = std::clamp((int32_t)std::floor(m_currentScrollOffset), 0, (int32_t)m_itemsSource->size() - 1);

				//第一个被生成的 item 的长度
				float firstItemLength = 0.0f;

				//生成控件，基于情况 a
				bool bHasFilledAvailableArea = false;//是否把 view 都占满了
				bool bAtEndOfList = false;

				const float layoutScaleMultiplier = myGeometry.getAccumulateLayoutTransform().m_linear[0][0];//get scale
				TableViewDimensions myDimensions(this->m_orientation, myGeometry.getLocalSize());

				for (int32_t itemIndex = startIndex; !bHasFilledAvailableArea && itemIndex < m_itemsSource->size(); ++itemIndex)
				{
					const ItemType& curItem = (*m_itemsSource)[itemIndex];

					if (!ListTypeTraits<ItemType>::isPtrValid(curItem))
					{
						continue;
					}

					//生成的 item 长度
					const float itemLength = generateWidgetForItem(curItem, itemIndex, startIndex, layoutScaleMultiplier);

					const bool bIsFirstItem = itemIndex == startIndex;

					if (bIsFirstItem)
					{
						firstItemLength = itemLength;
					}

					//追踪在 view 里面的 items 的数量，包含分数部分
					if(bIsFirstItem)
					{
						//第一个 item 可能不是完整的可见 (但是不能超过1)
						//first item fraction scrolled into view 是 item 可见的百分比
						const float firstItemFractionScrolledIntoView = 1.0f - (float)std::max(doubleFractional(m_currentScrollOffset), 0.0);

						//获得长度
						const float firstItemLengthScrolledIntoView = itemLength * firstItemFractionScrolledIntoView;

						//可见的 item length 是可用的 list view 长度的百分比 (如果 item size 大于可用的 size ， 否则这个将大于 1)，否则使用 first item fraction scrolled into view (这个不会超过1)
						const float firstItemVisibleFraction = std::min(myDimensions.m_scrollAxis / firstItemLengthScrolledIntoView, firstItemFractionScrolledIntoView);

						itemsInView += firstItemVisibleFraction;
					}
					else if (viewLengthUsedSoFar + itemLength > myDimensions.m_scrollAxis)
					{
						//最后一个 item 可能不是完整的可见
						itemsInView += (myDimensions.m_scrollAxis - viewLengthUsedSoFar) / itemLength;
					}
					else
					{
						itemsInView += 1;
					}

					lengthGeneratedSoFar += itemLength;

					//对于第一个 item ，items in view <= 1.0f
					viewLengthUsedSoFar += (bIsFirstItem) ? itemLength * itemsInView : itemLength;

					bAtEndOfList = itemIndex >= m_itemsSource->size() - 1;

					if (bIsFirstItem && viewLengthUsedSoFar >= myDimensions.m_scrollAxis)
					{
						bHasFilledAvailableArea = true;
					}
					else
					{
						const float floatPrecisionOffset = 0.001f;
						bHasFilledAvailableArea = viewLengthUsedSoFar >= myDimensions.m_scrollAxis + floatPrecisionOffset;
					}
				}

				//情况b
				//我们可能因为我们已经在 items 的最后所停止，但是我们可能仍然有空间去填充
				if (bAtEndOfList && !bHasFilledAvailableArea)
				{
					double newScrollOffsetForBackfill = static_cast<double>(startIndex) + (lengthGeneratedSoFar - myDimensions.m_scrollAxis) / firstItemLength;

					for (int32_t itemIndex = startIndex - 1; lengthGeneratedSoFar < myDimensions.m_scrollAxis && itemIndex >= 0; --itemIndex)
					{
						const ItemType& curItem = (*m_itemsSource)[itemIndex];

						if (ListTypeTraits<ItemType>::isPtrValid(curItem))
						{
							const float itemLength = generateWidgetForItem(curItem, itemIndex, startIndex, layoutScaleMultiplier);

							if (lengthGeneratedSoFar + itemIndex > myDimensions.m_scrollAxis && itemIndex > 0.0f)
							{
								newScrollOffsetForBackfill = itemIndex + (lengthGeneratedSoFar + itemLength - myDimensions.m_scrollAxis) / itemLength;
							}

							lengthGeneratedSoFar += itemLength;
						}
					}

					return ReGenerateResults(newScrollOffsetForBackfill, lengthGeneratedSoFar, m_itemsSource->size() - newScrollOffsetForBackfill, true);
				}

				//length generated so far ，表示生成的所有 widgets 的总长度
				//items in view，比如4.3，有4.3个 item 在 view 里面
				return ReGenerateResults(m_currentScrollOffset, lengthGeneratedSoFar, itemsInView, false);
			}

			return ReGenerateResults(0.0f, 0.0f, 0.0f, false);
		}

		virtual void rebuildList()
		{
			m_widgetGenerator.clear();
			requestListRefresh();
		}

		virtual const std::vector<int32_t>& privateGetWiresNeededByDepth(int32_t itemIndexInList) const override
		{
			return emptyBitArray;
		}

		virtual int32_t privateGetNestingDepth(int32_t itemIndexInList) const override
		{
			return 0;
		}

		virtual bool privateIsLastChild(int32_t itemIndexInList) const override
		{
			return false;
		}

		virtual int32_t privateDoesItemHaveChildren(int32_t itemIndexInList) const override
		{
			return false;
		}

		virtual const ItemType* privateItemFromWidget(const ITableRow* theWidget) const override
		{
			auto it = m_widgetGenerator.m_widgetMapToItem.find(theWidget);
			return it == m_widgetGenerator.m_widgetMapToItem.end() ? nullptr : &(it->second);
		}

		virtual bool privateIsItemExpanded(const ItemType& theItem) const override
		{
			return false;
		}

		virtual void privateSetItemExpansion(ItemType theItem, bool bShouldBeExpanded) override
		{

		}

		virtual bool privateIsItemSelected(const ItemType& theItem) const override
		{
			return m_selectedItems.find(theItem) != m_selectedItems.end();
		}

		virtual bool privateIsItemHighlighted(const ItemType& theItem) const override
		{
			return m_highlightedItems.find(theItem) != m_highlightedItems.end();
		}

		virtual void privateSetItemSelection(ItemType theItem, bool bShouldBeSelected, bool bWasUserDirected = false) override
		{
			if (m_selectionMode.Get() == SelectionMode::None)
			{
				return;
			}

			if (bShouldBeSelected)
			{
				m_selectedItems.insert(theItem);
			}
			else
			{
				auto it = m_selectedItems.find(theItem);
				m_selectedItems.erase(it);
			}
		}

		virtual void privateClearSelection() override
		{
			m_selectedItems.clear();
		}

		virtual bool privateOnItemDoubleClicked(ItemType theItem) override
		{
			if (m_onDoubleClick)
			{
				m_onDoubleClick(theItem);
				return true;//handled
			}
			return false;//not handled
		}

		virtual void  privateSignalSelectionChanged(SelectInfo::Type selectInfo) override
		{
			if (m_selectionMode.Get() == SelectionMode::None)
			{
				return;
			}

			if (m_onSelectionChanged)
			{
				NullableItemType selectedItem = (m_selectedItems.size() > 0)
					? (*typename ItemSet::iterator(m_selectedItems.begin()))
					: ListTypeTraits<ItemType>::makeNullPtr();

				m_onSelectionChanged(selectedItem, selectInfo);
			}
		}

		virtual void privateSetItemHighlighted(ItemType theItem, bool bShouldBeHighted)
		{
			if (bShouldBeHighted)
			{
				m_highlightedItems.insert(theItem);
			}
			else
			{
				m_highlightedItems.erase(theItem);
			}
		}

		virtual void privateOnItemRightClicked(ItemType theItem, const PointerEvent& mouseEvent) override
		{
			this->onRightMouseButtonUp(mouseEvent);
		}

		virtual std::shared_ptr<Widget> asWidget() override
		{
			return shared_from_this();
		}

		//请求一个 item 滚进 view 里面
		void requestScrollIntoView(ItemType itemToView)
		{
			m_itemToScrollIntoView = itemToView;
			requestLayoutRefresh();
		}

		virtual std::vector<ItemType> getSelectedItems() const override
		{
			std::vector<ItemType> selectedItemArray;
			selectedItemArray.resize(m_selectedItems.size());
			for (typename ItemSet::const_iterator selectedItemIt(m_selectedItems.begin()); selectedItemIt != m_selectedItems.end(); ++selectedItemIt)
			{
				selectedItemArray.push_back(*selectedItemIt);
			}
			return selectedItemArray;
		}

		void clearSelection()
		{
			privateClearSelection();
		}

		void setItemSelection(const ItemType& inItem, bool bSelected, SelectInfo::Type selectInfo = SelectInfo::Direct)
		{
			if (m_selectionMode.Get() == SelectionMode::None)
			{
				return;
			}

			privateSetItemSelection(inItem, bSelected, selectInfo != SelectInfo::Direct);
			privateSignalSelectionChanged(selectInfo);
		}
	private:
		friend class WidgetGenerator;

		class WidgetGenerator
		{
		public:
			WidgetGenerator(ListView<ItemType>* inOwnerList)
			{}

			std::shared_ptr<ITableRow> getWidgetForItem(const ItemType& item) const
			{
				auto it = m_itemsToWidgetMap.find(item);

				if (it != m_itemsToWidgetMap.end())
					return it->second;
				return nullptr;
			}

			//追踪每个数据项还有相应的 widget ，在一个生成过程中
			//把 item 和对应 row 显示在屏幕上
			void onItemSeen(ItemType inItem, std::shared_ptr<ITableRow> inGeneratedWidget)
			{
				auto it = m_itemsToWidgetMap.find(inItem);
				const bool bWidgetIsNewlyGenerated = it == m_itemsToWidgetMap.end();
				if (bWidgetIsNewlyGenerated)
				{
					//新的项
					m_itemsToWidgetMap.insert({ inItem, inGeneratedWidget });
					m_widgetMapToItem.insert({ inGeneratedWidget.get(), inItem });

					//item-widget 的关系已经建立，生成的 row 可以完整地初始化它自己
					inGeneratedWidget->initializeRow();
				}

				auto it2 = std::find(m_itemsToBeCleanedUp.begin(), m_itemsToBeCleanedUp.end(), inItem);
				if (it2 != m_itemsToBeCleanedUp.end())
				{
					m_itemsToBeCleanedUp.erase(it2);
					m_itemsWithGeneratedWidgets.push_back(inItem);
				}
				
			}

			//被调用，在生成pass的开始
			void onBeginGenerationPass()
			{
				m_itemsToBeCleanedUp = m_itemsWithGeneratedWidgets;
				m_itemsWithGeneratedWidgets.clear();
			}

			void onEndGenerationPass()
			{
				processItemsCleanUp();
			}

			void clear()
			{
				m_itemsToBeCleanedUp = m_itemsWithGeneratedWidgets;
				m_itemsWithGeneratedWidgets.clear();
				processItemsCleanUp();
			}

			void processItemsCleanUp()
			{
				for (int32_t itemIndex = 0; itemIndex < m_itemsToBeCleanedUp.size(); ++itemIndex)
				{
					ItemType itemToBeCleanedUp = m_itemsToBeCleanedUp[itemIndex];
					auto findResult = m_itemsToWidgetMap.find(itemToBeCleanedUp);
					if (findResult != m_itemsToWidgetMap.end())
					{
						const std::shared_ptr<ITableRow> widgetToCleanUp = findResult->second;
						m_itemsToWidgetMap.erase(findResult);

						auto it = m_widgetMapToItem.find(widgetToCleanUp.get());
						m_widgetMapToItem.erase(it);

						if (m_ownerList != nullptr)
						{
							widgetToCleanUp->resetRow();
							//todo : 添加回调
						}
					}
					//todo : 添加指针的判断
				}

				m_itemsToBeCleanedUp.clear();
			}

			ListView<ItemType>* m_ownerList;

			//映射数据项到对应的 widget
			std::unordered_map<ItemType, std::shared_ptr<ITableRow>> m_itemsToWidgetMap;

			//映射 widget 到对应的数据项
			std::unordered_map<const ITableRow*, ItemType> m_widgetMapToItem;

			//已经有生成的 widget 的数据项的集合
			std::vector<ItemType> m_itemsWithGeneratedWidgets;

			//上一次我们执行生成过程的总的数据项的数量
			int32_t m_totalItemsLastGeneration;

			//需要清理掉对应 widgets 的数据项，当我们在屏幕上看不到的时候
			std::vector<ItemType> m_itemsToBeCleanedUp;
		};

	protected:
		WidgetGenerator m_widgetGenerator;

		//当列表需要从一个数据 item 生成一个新的 widget 的时候，被调用的委托
		OnGenerateRow m_onGenerateRow;

		OnMouseButtonDoubleClick m_onDoubleClick;

		//指向我们正在观察的数据项的数组的指针
		const std::vector<ItemType>* m_itemsSource;

		//是否 list view 支持键盘焦点
		Attribute<bool> m_isFocusable;

		//当不为 null ，列表将尝试去滑动这个 item 在tick的时候
		NullableItemType m_itemToScrollIntoView;

		//当设置，列表将会通知这个 item 当它滑进 view 里面
		NullableItemType m_itemToNotifyWhenInView;

		bool m_bNavigateOnScrollIntoView = false;
		//选中的集合
		ItemSet m_selectedItems;

		//高亮的集合
		ItemSet m_highlightedItems;

		Attribute<SelectionMode::Type> m_selectionMode;

		OnSelectionChanged m_onSelectionChanged;
	private:
		struct GenerationPassGuard
		{
			WidgetGenerator& m_generator;
			GenerationPassGuard(WidgetGenerator& inGenerator)
				: m_generator(inGenerator)
			{
				m_generator.onBeginGenerationPass();
			}

			~GenerationPassGuard()
			{
				m_generator.onEndGenerationPass();
			}
		};
	};

}