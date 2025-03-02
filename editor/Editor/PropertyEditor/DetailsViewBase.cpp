#include <pch.h>

#include "DetailsViewBase.h"

namespace GuGu {

	DetailsViewBase::DetailsViewBase()
	{

	}

	DetailsViewBase::~DetailsViewBase()
	{

	}

	void DetailsViewBase::onGetChildrenForDetailTree(std::shared_ptr<DetailTreeNode> inTreeNode, std::vector<std::shared_ptr<DetailTreeNode>>& outChildren)
	{
		return inTreeNode->getChildren(outChildren);
	}

	std::shared_ptr<ITableRow> DetailsViewBase::onGenerateRowForDetailTree(std::shared_ptr<DetailTreeNode> inTreeNode, const std::shared_ptr<TableViewBase>& ownerTable)
	{
		return inTreeNode->generateWidgetForTableView(ownerTable);
	}

}