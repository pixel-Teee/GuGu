#pragma once

#include "TableViewBase.h" //table view base
#include "WidgetDelegates.h"

namespace GuGu {
	template<typename ItemType>
	class ListView : public TableViewBase
	{
	public:

		using OnGenerateRow = typename WidgetDelegates<ItemType>::OnGenerateRow;

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

	protected:
		//当列表需要从一个数据 item 生成一个新的 widget 的时候，被调用的委托
		OnGenerateRow m_onGenerateRow;

		//指向我们正在观察的数据项的数组的指针
		const std::vector<ItemType>* m_itemsSource;
	};
}