#pragma once

#include "IDetailTreeNode.h"
#include <Core/GuGuUtf8Str.h>
#include <vector>

namespace GuGu {
	class DetailTreeNode;//detail tree node
	class ITableRow;
	class TableViewBase;
	using DetailNodeList = std::vector<std::shared_ptr<DetailTreeNode>>;
	class DetailTreeNode : public IDetailTreeNode
	{
	public:
		virtual ~DetailTreeNode() {}

		virtual void getChildren(DetailNodeList& outChildren) = 0;

		virtual std::shared_ptr<ITableRow> generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable) = 0;

		virtual GuGuUtf8Str getNodeName() const = 0;

		void setParentNode(std::weak_ptr<DetailTreeNode> inParentNode) { m_parentNode = inParentNode; }
	private:
		std::weak_ptr<DetailTreeNode> m_parentNode;
	};
}