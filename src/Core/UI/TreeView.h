#pragma once

#include "ListView.h"

#include <bitset>
#include <set>
#include <unordered_set>
//#include "Clipping.h"
#include <Core/UI/CoreStyle.h>

namespace GuGu {
	struct SparseItemInfo
	{
		SparseItemInfo(bool inIsExpanded, bool inHasExpandedChildren)
			: m_bIsExpanded(inIsExpanded)
			, m_bHasExpandedChildren(inHasExpandedChildren)
		{}
		//这个 tree item 是否展开？
		bool m_bIsExpanded;

		//这个 tree item 是否有其他展开的孩子？
		bool m_bHasExpandedChildren;
	};
	//通过每个在 tree 中可见的 item 提供
	struct ItemInfo
	{
		ItemInfo()
		{

		}

		ItemInfo(std::vector<int32_t> inNeedsVerticalWire, bool inHasChildren, bool inIsLastChild, int32_t inParentIndex)
			: m_needsVerticalWire(inNeedsVerticalWire)
			, m_bHasChildren(inHasChildren)
			, m_bIsLastChild(inIsLastChild)
			, m_parentIndex(inParentIndex)
		{}

		//是否我们需要一个拉丝线对于这个树的层级的集合
		//0表示根层级，1是根的儿子，等等
		std::vector<int32_t> m_needsVerticalWire;

		int32_t getNesingLevel() const { return m_needsVerticalWire.size() - 1; }

		uint32_t m_bHasChildren;//这个tree item 是否有孩子

		uint32_t m_bIsLastChild;//是否是父亲的最后一个孩子

		int32_t m_parentIndex;//指向线性树的父亲，对于这个item，如果没有，设置为-1
	};
	template<typename ItemType>
	class TreeView : public ListView<ItemType>
	{
	public:
		using NullableItemType = typename ListTypeTraits<ItemType>::NullableType;
		using OnGenerateRow = typename WidgetDelegates<ItemType>::OnGenerateRow;
		using OnGetChildren = typename WidgetDelegates<ItemType>::OnGetChildren;
		using OnExpansionChanged = typename WidgetDelegates<ItemType>::OnExpansionChanged;
		using OnSelectionChanged = typename WidgetDelegates<ItemType>::OnSelectionChanged;
		using ItemSet = std::unordered_set<ItemType>;
		using SparseItemMap = std::unordered_map<ItemType, SparseItemInfo>;
	public:
		TreeView() : ListView<ItemType>(TableViewMode::Tree)
			, m_treeItemsSource(nullptr)
			, m_bTreeItemsAreDirty(true)
		{
			this->m_itemsSource = &m_linearizedItems;//这里很关键，到时候基类 list view 会使用 items source 去生成 widget
		}

		virtual ~TreeView() {}

		struct BuilderArguments : public Arguments<TreeView>
		{
			BuilderArguments()
				: mstyle(CoreStyle::getStyleSet()->template getStyle<ScrollBarStyle>("ScrollBar"))
				, mitemHeight(16)
				, morientation(Orientation::Vertical)
				, mtreeItemSource()
				, mselectionMode(SelectionMode::Single)
			{
				this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, style)

			UI_EVENT(OnGenerateRow, onGenerateRow)

			UI_EVENT(OnGetChildren, onGetChildren)

			UI_EVENT(OnExpansionChanged, onExpansionChanged)

			UI_EVENT(OnTableViewScrolled, onTreeViewScrolled)

			UI_EVENT(OnSelectionChanged, onSelectionChanged)

			ARGUMENT_ATTRIBUTE(SelectionMode::Type, selectionMode)

			ARGUMENT_ATTRIBUTE(float, itemHeight)

			ARGUMENT_VALUE(std::shared_ptr<HeaderRow>, headerRow)

			ARGUMENT_VALUE(std::shared_ptr<ScrollBar>, externalScrollbar) //可以控制 list view 的外部的 scroll bar

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_VALUE(const std::vector<ItemType>*, treeItemSource)
		};

		void init(const BuilderArguments& arguments)
		{
			Widget::m_widgetClipping = arguments.mClip;
			//制作table view
			this->m_onGenerateRow = arguments.monGenerateRow;
			this->m_onGetChildren = arguments.monGetChildren;
			this->m_treeItemsSource = arguments.mtreeItemSource;//利用这个 tree items source 去生成 list view 的 items source
			this->m_onExpansionChanged = arguments.monExpansionChanged;
			this->m_onSelectionChanged = arguments.monSelectionChanged;
			this->m_selectionMode = arguments.mselectionMode;

			TableViewBase::constructChildren(0, arguments.mitemHeight, ListItemAlignment::LeftAligned, arguments.mheaderRow, arguments.mexternalScrollbar,
				arguments.morientation, arguments.monTreeViewScrolled, arguments.mstyle);

			//todo:可能这里以后要简化
			//Widget::m_childWidget->m_parentWidget = shared_from_this();
			//Widget::m_childWidget->m_childWidget->setParentWidget(shared_from_this());
			Widget::m_visibilityAttribute = arguments.mVisibility;
		}

		bool populateLinearizedItems(
			const std::vector<ItemType>& inItemsSource,
			std::vector<ItemType>& inLinearizedItems, //需要填充的 list view 的 items
			std::vector<ItemInfo>& newDenseItemInfos, //添加进list view 的 items 的 item info
			std::vector<int32_t> needsParentWire,//记录每个 item 的递归深度
			ItemSet& outNewSelectedItems,
			SparseItemMap& newSparseItemInfo,
			bool bAddingItems,
			int32_t parentIndex
		)
		{
			needsParentWire.push_back(false);
			const int32_t nestingDepthIndex = needsParentWire.size() - 1;

			bool bSawExpandedItems = false;
			for (int32_t itemIndex = 0; itemIndex < inItemsSource.size(); ++itemIndex)
			{
				const ItemType& curItem = inItemsSource[itemIndex];

				//if (curItem.find("1302") != -1)
				//{
				//	GuGu_LOGD("1302");
				//}

				//找到这个 items 的儿子
				std::vector<ItemType> childItems;
				m_onGetChildren(inItemsSource[itemIndex], childItems);

				const bool bHasChildren = childItems.size() > 0;

				//child items 将需要一个父拉丝线在这个深度
				const bool bIsLastChild = (itemIndex == inItemsSource.size() - 1);
				needsParentWire[nestingDepthIndex] = !bIsLastChild;

				//如果 m_spareItemInfos 里面包含这个，则代表这个 item 不可见
				auto it = m_spareItemInfos.find(curItem);
				const bool bIsExpanded = (it == m_spareItemInfos.end()) ? false : it->second.m_bIsExpanded;
				bool bHasExpandedChildren = (it == m_spareItemInfos.end()) ? false : it->second.m_bHasExpandedChildren;

				//const bool bIsExpanded = true;
				//bool bHasExpandedChildren = true;

				//添加这个 item 到 linearized list 还有更新选择的集合
				if (bAddingItems)
				{
					inLinearizedItems.push_back(curItem);

					newDenseItemInfos.push_back(ItemInfo(needsParentWire, bHasChildren, bIsLastChild, parentIndex));

					const bool bIsSelected = false;//todo:添加 is item selected
					if (bIsSelected)
					{
						outNewSelectedItems.insert(curItem);
					}
				}

				if (bIsExpanded || bHasExpandedChildren)
				{
					const bool bAddChildItems = bAddingItems && bIsExpanded;
					bHasExpandedChildren = populateLinearizedItems(childItems, 
						inLinearizedItems,
						newDenseItemInfos,
						needsParentWire,
						outNewSelectedItems,
						newSparseItemInfo,
						bAddChildItems,
						inLinearizedItems.size() - 1);
				}

				if (bIsExpanded || bHasExpandedChildren)
				{
					newSparseItemInfo.insert({ curItem, SparseItemInfo(bIsExpanded, bHasExpandedChildren) });
				}

				bSawExpandedItems = bSawExpandedItems || bIsExpanded || bHasExpandedChildren;
			}

			return bSawExpandedItems;
		}

		virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override
		{
			if (m_bTreeItemsAreDirty)
			{
				if (this->m_itemsPanel)
				{
					m_bTreeItemsAreDirty = false;

					//准备填充线性 items

					if (m_onGetChildren && m_treeItemsSource != nullptr)
					{
						//拷贝原先老的展开还有选择的集合，这样，我们可以移除任何不再 tree 可见的 items
						ItemSet tempSelectedItemsMap;
						SparseItemMap tempSparseItemInfo;
						std::vector<ItemInfo> tempDenseItemInfos;//dense:密集的

						//重新构建 tree data 的线性 view
						//m_linearized items 是我们要填充的
						m_linearizedItems.clear();
						populateLinearizedItems(*m_treeItemsSource, m_linearizedItems, tempDenseItemInfos, std::vector<int32_t>(),
							tempSelectedItemsMap, tempSparseItemInfo, true, -1);

						m_spareItemInfos = std::move(tempSparseItemInfo);
						m_denseItemInfos = std::move(tempDenseItemInfos);

					}
				}
			}

			ListView<ItemType>::Tick(allocatedGeometry, inCurrentTime, inDeltaTime);
		}

		virtual const std::vector<int32_t>& privateGetWiresNeededByDepth(int32_t itemIndexInList) const override
		{
			if (itemIndexInList >= 0 && itemIndexInList < m_denseItemInfos.size())
			{
				return m_denseItemInfos[itemIndexInList].m_needsVerticalWire;
			}
			return emptyBitArray;
		}

		virtual int32_t privateGetNestingDepth(int32_t itemIndexInList) const override
		{
			int32_t nestingLevel = 0;
			if (itemIndexInList >= 0 && itemIndexInList < m_denseItemInfos.size())
			{
				nestingLevel = m_denseItemInfos[itemIndexInList].getNesingLevel();
			}

			return nestingLevel;
		}

		virtual bool privateIsLastChild(int32_t itemIndexInList) const override
		{
			if (itemIndexInList >= 0 && itemIndexInList < m_denseItemInfos.size())
			{
				return m_denseItemInfos[itemIndexInList].m_bIsLastChild;
			}
			return false;
		}

		virtual int32_t privateDoesItemHaveChildren(int32_t itemIndexInList) const override
		{
			bool bHasChildren = false;
			if (itemIndexInList >= 0 && itemIndexInList < m_denseItemInfos.size())
			{
				return bHasChildren = m_denseItemInfos[itemIndexInList].m_bHasChildren;
			}
			return bHasChildren;
		}

		virtual bool privateIsItemExpanded(const ItemType& theItem) const override
		{
			auto it = m_spareItemInfos.find(theItem);
			if (it != m_spareItemInfos.end())
				return it->second.m_bIsExpanded;
			return false;
		}

		virtual void privateSetItemExpansion(ItemType theItem, bool bShouldBeExpanded)
		{
			const auto sparseItemInfo = m_spareItemInfos.find(theItem);

			bool bWasExpanded = false;

			if (sparseItemInfo != m_spareItemInfos.end())
			{
				bWasExpanded = sparseItemInfo->second.m_bIsExpanded;
				//m_spareItemInfos.insert({ theItem, SparseItemInfo(bShouldBeExpanded, sparseItemInfo->second.m_bHasExpandedChildren) });
				sparseItemInfo->second = SparseItemInfo(bShouldBeExpanded, sparseItemInfo->second.m_bHasExpandedChildren);
			}
			else if (bShouldBeExpanded)
			{
				m_spareItemInfos.insert({ theItem, SparseItemInfo(bShouldBeExpanded, false) });
			}

			if (bWasExpanded != bShouldBeExpanded)
			{
				if (m_onExpansionChanged)
					m_onExpansionChanged(theItem, bShouldBeExpanded);//展开通知
				requestTreeRefresh();
			}
		}

		virtual void requestListRefresh() override
		{
			m_bTreeItemsAreDirty = true;
			ListView<ItemType>::requestListRefresh();
		}

		void requestTreeRefresh()
		{
			requestListRefresh();
		}

		virtual void  privateSignalSelectionChanged(SelectInfo::Type selectInfo) override
		{
			ListView<ItemType>::privateSignalSelectionChanged(selectInfo);
		}

		void setItemExpansion(const ItemType& inItem, bool inShouldExpandItem)
		{
			privateSetItemExpansion(inItem, inShouldExpandItem);
		}

		bool isItemExpanded(const ItemType& inItem) const
		{
			return privateIsItemExpanded(inItem);
		}

	protected:
		//被调用的委托，当我们需要去收集一个 item 的儿子的时候
		OnGetChildren m_onGetChildren;

		//指向被 tree view 观测的 items 的指针
		const std::vector<ItemType>* m_treeItemsSource;

		std::vector<ItemType> m_linearizedItems;

		//tree items 的一小部分，控制一个 item 是否展开，以及是否有展开的儿子
		SparseItemMap m_spareItemInfos;

		std::vector<ItemInfo> m_denseItemInfos;

		OnExpansionChanged m_onExpansionChanged;
	private:
		//当 linearized items 需要被生成的时候，这个值为 true
		bool m_bTreeItemsAreDirty = false;
	};
}