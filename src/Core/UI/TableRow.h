#pragma once

#include "Border.h"
#include "ITableRow.h"
#include "ITypedTableView.h"
#include "TableViewBase.h"

namespace GuGu {
	class TableViewBase;
    //class ListView;
    template <typename ItemType> class ListView;
	template<typename ItemType>
	class TableRow : public ITableRow, public Border
	{
	public:
		struct BuilderArguments : public Arguments<TableRow<ItemType>>
		{
			BuilderArguments()
				//: mContent()
				: mpadding(Padding(0))
			{
				//this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_ATTRIBUTE(Padding, padding)
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
			}
		}
	protected:
		void initInternal(const BuilderArguments& inArgs, const std::shared_ptr<TableViewBase>& inOwnerTableView)
		{
			setOwnerTableView(inOwnerTableView);
		}
		//在 list 的相应的 data item 的索引
		int32_t m_indexInList;

		std::weak_ptr<ITypedTableView<ItemType>> m_ownerTablePtr;

		std::weak_ptr<Widget> m_content;
	};
}