#include <pch.h>

#include  "DetailSingleItemRow.h"

namespace GuGu {

	void DetailSingleItemRow::init(const BuilderArguments& inArgs, DetailLayoutCustomization* inCustomization, bool bHasMultipleColumns, std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView)
	{
		m_ownerTreeNode = inOwnerTreeNode;

		std::shared_ptr<Widget> widget = NullWidget::getNullWidget();
		const bool bIsValidTreeNode = true;

		if (bIsValidTreeNode)
		{
			
		}
	}

}