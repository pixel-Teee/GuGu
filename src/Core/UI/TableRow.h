#pragma once

#include "Border.h"
#include "ITableRow.h"
#include "ITypedTableView.h"
#include "TableViewBase.h"
#include "ExpanderArrow.h"
#include "BoxPanel.h"
#include "CoreStyle.h"
#include "WidgetDelegates.h"
#include "NullWidget.h"
#include "Box.h"

namespace GuGu {
	class TableViewBase;
    //class ListView;
    template<typename ItemType> class ListView;
	template<typename ItemType>
	class TableRow : public ITableRow, public Border
	{
	public:
		struct BuilderArguments : public Arguments<TableRow<ItemType>>
		{
			BuilderArguments()
				//: mContent()
				: mStyle(CoreStyle::getStyleSet()->template getStyle<TableRowStyle>("tableView.Row"))
				, mpadding(Padding(0))
			{
				//this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<TableRowStyle>, Style)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_ATTRIBUTE(Padding, padding)

			UI_EVENT(GOnDragDetected, onDragDetected)
		};

		void init(const typename TableRow<ItemType>::BuilderArguments& arguments, const std::shared_ptr<TableViewBase>& inOwnerTableView)
		{
			initInternal(arguments, inOwnerTableView);

			initChildren(inOwnerTableView->m_tableViewMode, arguments.mpadding, arguments.mContent->getChildWidget());
		}

		virtual void initializeRow() override {}

		virtual void resetRow() override {}

		virtual void setIndexInList(int32_t inIndexInList) override
		{
			m_indexInList = inIndexInList;
		}

		virtual std::shared_ptr<Widget> asWidget() override
		{
			return shared_from_this();
		}

		void setOwnerTableView(std::shared_ptr<TableViewBase> ownerTableView)
		{
			this->m_ownerTablePtr = std::static_pointer_cast<ITypedTableView<ItemType>>(std::static_pointer_cast<ListView<ItemType>>(ownerTableView));
			//this->m_ownerTablePtr =
		}

		virtual void initChildren(TableViewMode::Type inOwnerTableMode, const Attribute<Padding>& inPadding, const std::shared_ptr<Widget>& inContent)
		{
			this->m_content = inContent;

			if (inOwnerTableMode == TableViewMode::List || inOwnerTableMode == TableViewMode::Tile)
			{
				this->m_childWidget = std::make_shared<SingleChildSlot>();
				this->m_childWidget->setChildWidget(inContent);
				inContent->setParentWidget(shared_from_this());
				this->m_childWidget->setPadding(inPadding.Get());
			}
			else
			{
				//tree view

				//row is for tree view
				HorizontalBox::HorizontalBoxSlot* innerContentSlotNativePtr = nullptr;

				//rows in a tree view need a expander button and some indentation
				this->m_childWidget = std::make_shared<SingleChildSlot>();
				
				//todo: 添加 expander arrow
				std::shared_ptr<HorizontalBox> box;
				this->m_childWidget->setChildWidget(
					WIDGET_ASSIGN_NEW(HorizontalBox, box)
					+ HorizontalBox::Slot()
					.FixedWidth()
					.setHorizontalAlignment(HorizontalAlignment::Right)
					.setVerticalAlignment(VerticalAlignment::Stretch)
					(
						WIDGET_NEW(ExpanderArrow, std::static_pointer_cast<TableRow>(shared_from_this()))
						//.shouldDrawWires(true)
					)
					+ HorizontalBox::Slot()
					.StretchWidth(1.0f)
					.expose(innerContentSlotNativePtr)
					.setPadding(inPadding.Get())
					(
						inContent
					)
				);

				box->setParentWidget(shared_from_this());

				m_innerContentSlot = innerContentSlotNativePtr;
			}
		}

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();
			m_bChangedSelectionOnMouseDown = false;
			m_bDragWasDected = false;

			if (inMouseEvent.m_effectingButton == Keys::LeftMouseButton)
			{
				const SelectionMode::Type selectionMode = SelectionMode::Type::Single;
				if (selectionMode != SelectionMode::None)
				{
					if (const ItemType* myItemPtr = getItemForThis(ownerTable))
					{
						const ItemType& myItem = *myItemPtr;
						const bool bIsSelected = ownerTable->privateIsItemSelected(myItem);

						if (!bIsSelected)
						{
							ownerTable->privateClearSelection();
							ownerTable->privateSetItemSelection(myItem, true, true);
							m_bChangedSelectionOnMouseDown = true;
							ownerTable->privateSignalSelectionChanged(SelectInfo::Direct);
						}

						return Reply::Handled().setFocus(ownerTable->asWidget()).captureMouse(shared_from_this())
						.detectDrag(shared_from_this(), Keys::LeftMouseButton);
					}
				}
			}
			else if (inMouseEvent.getEffectingButton() == Keys::RightMouseButton)
			{
				if (const ItemType* myItemIt = getItemForThis(ownerTable))
				{
					const bool bIsSelected = ownerTable->privateIsItemSelected(*myItemIt);

					if (!bIsSelected)
					{
						ownerTable->privateClearSelection();
						ownerTable->privateSetItemSelection(*myItemIt, true, true);
						ownerTable->privateSignalSelectionChanged(SelectInfo::OnMouseClick);
					}

					ownerTable->privateOnItemRightClicked(*myItemIt, inMouseEvent);

					return Reply::Handled();
				}
			}

			return Reply::Unhandled();
		}

		virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();

			//note:这个转换需要ListView的定义，但是为了避免循环包含，所以包含 table row 的时候，需要包含 list view
			std::shared_ptr<TableViewBase> ownerTableViewBase = std::static_pointer_cast<ListView<ItemType>>(ownerTable);

			if(inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
			{
				Reply reply = Reply::Unhandled().releaseMouseCapture();

				if (m_bChangedSelectionOnMouseDown)
				{
					reply = Reply::Handled().releaseMouseCapture();
				}

				const bool bIsUnderMouse = myGeometry.isUnderLocation(inMouseEvent.m_screenSpacePosition);

				if (hasMouseCapture())
				{
					if (bIsUnderMouse && !m_bDragWasDected)
					{
						switch (getSelectionMode())
						{
							case SelectionMode::SingleToggle:
							{
								if (!m_bChangedSelectionOnMouseDown)
								{
									ownerTable->privateClearSelection();
									ownerTable->privateSignalSelectionChanged(SelectInfo::OnMouseClick);
								}
								reply = Reply::Handled().releaseMouseCapture();
								break;
							}
						}

						if (const ItemType* myItemPtr = getItemForThis(ownerTable))
						{
							if (ownerTable->privateOnItemClicked(*myItemPtr))
							{
								reply = Reply::Handled().releaseMouseCapture();
							}
						}

						if (m_bChangedSelectionOnMouseDown && !m_bDragWasDected)
						{
							ownerTable->privateSignalSelectionChanged(SelectInfo::OnMouseClick);
						}

						return reply;
					}
				}
			}

			return Reply::Unhandled();
		}

		virtual Reply OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();

			if (inMouseEvent.m_effectingButton == Keys::LeftMouseButton)
			{
				if (const ItemType* myItemPtr = getItemForThis(ownerTable))
				{
					const ItemType& myItem = *myItemPtr;
					const bool bWasHandled = ownerTable->privateOnItemDoubleClicked(myItem);
					if (!bWasHandled)
					{
						toggleExpansion();
					}
					return Reply::Handled();
				}
			}

			return Reply::Unhandled();
		}

		//这个很重要，通过这个函数，获取这个 item 所在的深度
		virtual std::vector<int32_t> getWiresNeededByDepth() const override
		{
			return m_ownerTablePtr.lock()->privateGetWiresNeededByDepth(m_indexInList);
		}

		virtual int32_t getIndentLevel() const override
		{
			return m_ownerTablePtr.lock()->privateGetNestingDepth(m_indexInList);
		}

		virtual bool isLastChild() const override
		{
			return m_ownerTablePtr.lock()->privateIsLastChild(m_indexInList);
		}

		virtual int32_t doesItemHaveChildren() const override
		{
			return m_ownerTablePtr.lock()->privateDoesItemHaveChildren(m_indexInList);
		}

		const ItemType* getItemForThis(const std::shared_ptr<ITypedTableView<ItemType>>& ownerTable) const
		{
			const ItemType* myItemPtr = m_ownerTablePtr.lock()->privateItemFromWidget(this);
			if (myItemPtr)
			{
				return myItemPtr;
			}
			else
			{
				return nullptr;
			}

			return nullptr;
		}

		virtual bool isItemExpanded() const override
		{
			std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();

			if (const ItemType* myItemPtr = getItemForThis(ownerTable))
			{
				return ownerTable->privateIsItemExpanded(*myItemPtr);
			}

			return false;
		}

		virtual void toggleExpansion() override
		{
			std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();

			const bool bItemHasChildren = ownerTable->privateDoesItemHaveChildren(m_indexInList);

			if (bItemHasChildren)
			{
				if (const ItemType* myItemPtr = getItemForThis(ownerTable))
				{
					const bool bIsItemExpanded = bItemHasChildren && m_ownerTablePtr.lock()->privateIsItemExpanded(*myItemPtr);
					m_ownerTablePtr.lock()->privateSetItemExpansion(*myItemPtr, !bIsItemExpanded);
				}
			}
		}

		virtual Reply OnDragDetected(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent) override
		{
			if (hasMouseCapture())
			{

			}
			
			if (m_onDragDectedHandler)
			{
				return m_onDragDectedHandler(myGeometry, mouseEvent);
			}
			else
			{
				return Reply::Unhandled();
			}
		}

		virtual SelectionMode::Type getSelectionMode() const override
		{
			const std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();
			return ownerTable->privateGetSelectionMode();
		}
	protected:
		void initInternal(const BuilderArguments& inArgs, const std::shared_ptr<TableViewBase>& inOwnerTableView)
		{
			m_style = inArgs.mStyle.Get();

			m_onDragDectedHandler = inArgs.monDragDetected;

			setBorderImage(Attribute<std::shared_ptr<Brush>>(this, &TableRow::getBorder));

			setOwnerTableView(inOwnerTableView);
		}

		virtual std::shared_ptr<Brush> getBorder() const
		{
			std::shared_ptr<ITypedTableView<ItemType>> ownerTable = m_ownerTablePtr.lock();

			const bool bIsActive = ownerTable->asWidget()->hasKeyboardFocus();//有焦点

			const bool bItemHasChildren = ownerTable->privateDoesItemHaveChildren(m_indexInList);

			if (const ItemType* myItemPtr = getItemForThis(ownerTable))
			{
				const bool bIsSelected = ownerTable->privateIsItemSelected(*myItemPtr);
				const bool bIsHighlighted = ownerTable->privateIsItemHighlighted(*myItemPtr);
				
				const bool bAllowSelection = true;//todo:修复这里
				const bool bEvenEntryIndex = (m_indexInList % 2 == 0);//是否是偶数的索引，来更换样式
				
				if (bIsSelected) //被选中
				{
					if (bIsActive) //被选中，并且有键盘焦点
					{
						return IsHovered()
							? m_style->m_activeHoveredBrush
							: m_style->m_activeBrush;
					} 
					else //被选中，没有键盘焦点
					{
						return IsHovered()
							? m_style->m_inActiveHoveredBrush
							: m_style->m_inactiveBrush;
					}
				}
				else if(!bIsSelected && bIsHighlighted) //没被选中，但是有高光，相比于没高光的，普通状态不一样
				{
					if (bIsActive) //有键盘焦点
					{
						return IsHovered()
							? (bEvenEntryIndex ? m_style->m_evenRowBackgroundHoveredBrush : m_style->m_oddRowBackgroundHoveredBrush)
							: m_style->m_activeHighlightedBrush; //有键盘焦点的高亮
					}
					else
					{
						return IsHovered()
							? (bEvenEntryIndex ? m_style->m_evenRowBackgroundBrush : m_style->m_oddRowBackgroundBrush)
							: m_style->m_inactiveHighlightedBrush; //无键盘焦点的高亮
					}
				}
				else
				{
					if (bEvenEntryIndex)
					{
						return IsHovered()
							? m_style->m_evenRowBackgroundHoveredBrush
							: m_style->m_evenRowBackgroundBrush;
					}
					else
					{
						return IsHovered()
							? m_style->m_oddRowBackgroundHoveredBrush
							: m_style->m_oddRowBackgroundBrush;
					}
				}
				
			}

			return nullptr;
		}

		//在 list 的相应的 data item 的索引
		int32_t m_indexInList;

		std::weak_ptr<ITypedTableView<ItemType>> m_ownerTablePtr;

		std::weak_ptr<Widget> m_content;

		SlotBase* m_innerContentSlot;

		std::shared_ptr<TableRowStyle> m_style;

		GOnDragDetected m_onDragDectedHandler;

		bool m_bChangedSelectionOnMouseDown;

		bool m_bDragWasDected;
	};

	template<typename ItemType>
	class MultiColumnTableRow : public TableRow<ItemType>
	{
	public:
		virtual std::shared_ptr<Widget> generateWidgetForColumn(const GuGuUtf8Str& inColumnName) = 0;

		typedef MultiColumnTableRow<ItemType> SuperRowType;

		typedef typename TableRow<ItemType>::BuilderArguments TableRowArgs;

	protected:
		void init(const TableRowArgs& arguments, const std::shared_ptr<TableViewBase>& inOwnerTableView)
		{
			TableRow<ItemType>::init(
				TableRowArgs()
				.Style(arguments.mStyle)
				.padding(arguments.mpadding)
				.onDragDetected(arguments.monDragDetected)
				.Content
				(
					WIDGET_ASSIGN_NEW(HorizontalBox, m_box)
				),
				inOwnerTableView
			);

			std::shared_ptr<HeaderRow> headerRow = inOwnerTableView->getHeaderRow();//header row
			
			this->generateColumns(headerRow);
		}

		void generateColumns(const std::shared_ptr<HeaderRow>& inColumnHeaders)
		{
			m_box->clearChildren();
			const std::vector<std::shared_ptr<HeaderRow::GColumn>>& columns = inColumnHeaders->getColumns();//中间管理列类型的数据
			const int32_t numColumns = columns.size();
			std::map<GuGuUtf8Str, std::shared_ptr<Widget>> newColumnIdToSlotContents;

			for (int32_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
			{
				const HeaderRow::GColumn& column = *columns[columnIndex];
				if (column.m_shouldGenerateWidget.Get(true))
				{
					auto it = m_columnIdToSlotContents.find(column.m_columnId);
					std::shared_ptr<Widget> existingWidget = nullptr;
					std::shared_ptr<Widget> cellContents = NullWidget::getNullWidget();
					if (it != m_columnIdToSlotContents.end())
					{
						existingWidget = it->second;
						cellContents = existingWidget;
					}
					else
					{
						cellContents = generateWidgetForColumn(column.m_columnId);//生成列的内容
					}

					if (cellContents != NullWidget::getNullWidget())
					{
						cellContents->setClipping(WidgetClipping::OnDemand);
					}

					switch (column.m_sizeRule)
					{
						case ColumnSizeMode::Fill:
						{
							Attribute<float> widthBinding;
							widthBinding.bindRaw(&column, &HeaderRow::GColumn::getWidth);

							m_box->addSlot()
							.setHorizontalAlignment(column.m_cellHAlignment)
							.setVerticalAlignment(column.m_cellVAlignment)
							.StretchWidth(widthBinding)
							(
								cellContents
							);		
							break;
						}
						case ColumnSizeMode::Fixed:
						{
							m_box->addSlot()
							.FixedWidth()
							(
								WIDGET_NEW(BoxWidget)
								.WidthOverride(column.m_width.Get())
								.HAlign(column.m_cellHAlignment)
								.VAlign(column.m_cellVAlignment)
								.Clip(WidgetClipping::OnDemand)
								.Content
								(
									cellContents
								)
							);
							break;
						}
						case ColumnSizeMode::Manual:
						{
							auto getColumnWidthAsOptionalSize = [&column]()->OptionalSize
							{
								const float fixedWidth = column.getWidth();
								return OptionalSize(fixedWidth);
							};

							Attribute<OptionalSize> widthBinding;
							widthBinding.bind(getColumnWidthAsOptionalSize);

							m_box->addSlot()
							.FixedWidth()
							(
								WIDGET_NEW(BoxWidget)
								.HAlign(column.m_cellHAlignment)
								.VAlign(column.m_cellVAlignment)
								.Clip(WidgetClipping::OnDemand)
								.Content
								(
									cellContents
								)
							);
							break;
						}
					}

					newColumnIdToSlotContents.insert({column.m_columnId, cellContents});
				}
			}
			m_columnIdToSlotContents = newColumnIdToSlotContents;
		}

	private:
		std::shared_ptr<HorizontalBox> m_box;
		std::map<GuGuUtf8Str, std::shared_ptr<Widget>> m_columnIdToSlotContents;
	};
}