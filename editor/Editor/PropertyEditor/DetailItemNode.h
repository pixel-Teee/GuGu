#pragma once

#include "DetailTreeNode.h"

namespace GuGu {
	class DetailItemNode : public DetailTreeNode
	{
	public:
		DetailItemNode();

		virtual ~DetailItemNode();

		virtual void getChildren(DetailNodeList& outChildren) override;

		virtual std::shared_ptr<ITableRow> generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable) override;
	};
}