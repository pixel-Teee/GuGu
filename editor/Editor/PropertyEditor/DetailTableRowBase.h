#pragma once

#include <Core/UI/TableRow.h>
#include "DetailTreeNode.h"
#include <Core/UI/ListView.h>//包含 table row 的时候，需要包含 list view

namespace GuGu {
	class DetailTableRowBase : public TableRow<std::shared_ptr<DetailTreeNode>>
	{
	public:

	protected:
		std::weak_ptr<DetailTreeNode> m_ownerTreeNode;
	};
}