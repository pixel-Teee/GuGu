#pragma once

#include "IDetailsViewPrivate.h"
#include "DetailTreeNode.h"//DetailTreeNode
#include "PropertyRowGenerator.h"//RootPropertyNodeList
#include <Core/UI/TreeView.h>

namespace GuGu {
	using DetailTree = TreeView<std::shared_ptr<DetailTreeNode>>;
	class DetailsViewBase : public IDetailsViewPrivate
	{
	public:
		DetailsViewBase();
		virtual ~DetailsViewBase();

		void onGetChildrenForDetailTree(std::shared_ptr<DetailTreeNode> inTreeNode, std::vector<std::shared_ptr<DetailTreeNode>>& outChildren);

		std::shared_ptr<ITableRow> onGenerateRowForDetailTree(std::shared_ptr<DetailTreeNode> inTreeNode, const std::shared_ptr<TableViewBase>& ownerTable);

		//更新 details
		void updateFilteredDetails();

		void refreshTree();

		virtual RootPropertyNodeList& getRootNodes() = 0;
	protected:
		void updatePropertyMaps();

		//tree 中可见的 root tree nodes
		DetailNodeList m_rootTreeNodes;//用来构造 tree view

		//tree view
		std::shared_ptr<DetailTree> m_detailTree;

		DetailLayoutList m_detailLayouts;
	};
}