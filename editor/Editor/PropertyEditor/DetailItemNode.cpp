#include <pch.h>

#include "DetailItemNode.h"

namespace GuGu {

	DetailItemNode::DetailItemNode()
	{

	}

	DetailItemNode::~DetailItemNode()
	{

	}

	void DetailItemNode::getChildren(DetailNodeList& outChildren)
	{
		//todo:实现这个函数
	}

	std::shared_ptr<GuGu::ITableRow> DetailItemNode::generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable)
	{
		return nullptr;
	}

}