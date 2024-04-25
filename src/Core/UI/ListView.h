#pragma once

#include "TableViewBase.h" //table view base
#include "WidgetDelegates.h"

namespace GuGu {
	template<typename ItemType>
	class ListView : public TableViewBase, ITypedTableView<ItemType>
	{
	public:

		using OnGenerateRow = typename WidgetDelegates<ItemType>::OnGenerateRow;

		ListView(TableViewMode::Type inListMode = TableViewMode::List)
			: TableViewBase(inListMode)
			, m_itemsSource(nullptr)
			, m_isFocusable(true)
			, m_widgetGenerator(this)
		{

		}

		virtual ~ListView() {}

		struct BuilderArguments : public Arguments<ListView>
		{
			BuilderArguments()
				: mstyle(StyleSet::getStyle()->template getStyle<ScrollBarStyle>("ScrollBar"))
				, mitemHeight(16)
				, morientation(Orientation::Vertical)
			{
				this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, style)

			UI_EVENT(OnGenerateRow, onGenerateRow)

			UI_EVENT(OnTableViewScrolled, onListViewScrolled)

			ARGUMENT_ATTRIBUTE(float, itemHeight)

			ARGUMENT_VALUE(std::shared_ptr<HeaderRow>, headerRow)

			ARGUMENT_VALUE(std::shared_ptr<ScrollBar>, externalScrollbar) //可以控制 list view 的外部的 scroll bar

			ARGUMENT_VALUE(Orientation, orientation)
		};

		void init(const BuilderArguments& arguments)
		{
			m_widgetClipping = arguments.mClip;
			//制作table view
			m_onGenerateRow = arguments.monGenerateRow;

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
			if (this->m_itemsSource != nullptr)
			{

			}

			return ScrollIntoViewResult::Success;
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

				auto it = std::find(m_itemsToBeCleanedUp.begin(), m_itemsToBeCleanedUp.end(), inItem);
				m_itemsToBeCleanedUp.erase(it);
				m_itemsWithGeneratedWidgets.push_back(inItem);
			}

			//被调用，在生成pass的开始
			void onBeginGennerationPass()
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
					if (findResult != nullptr)
					{
						const std::shared_ptr<ITableRow> widgetToCleanUp = findResult.second;
						m_itemsToWidgetMap.erase(findResult);

						auto it = m_widgetMapToItem.find(widgetToCleanUp.get());
						m_widgetMapToItem.erase(it);

						if (m_ownerList != nullptr)
						{
							m_ownerList->resetRow();
							//todo : 添加回调
						}
					}
					//todo ：添加对指针的判断
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

		//指向我们正在观察的数据项的数组的指针
		const std::vector<ItemType>* m_itemsSource;

		//是否 list view 支持键盘焦点
		Attribute<bool> m_isFocusable;

	
	};
}