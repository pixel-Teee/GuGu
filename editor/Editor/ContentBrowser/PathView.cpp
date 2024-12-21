#include <pch.h>

#include "PathView.h"
#include "TreeItem.h"

namespace GuGu {
	void PathView::init(const BuilderArguments& arguments)
	{
		if (!m_treeViewPtr)
		{
			WIDGET_ASSIGN_NEW(TreeView<std::shared_ptr<TreeItem>>, m_treeViewPtr)
			.treeItemSource(&m_treeRootItems)
			.itemHeight(18)
			.onGenerateRow(this, &PathView::generateTreeRow)
			.onExpansionChanged(this, &PathView::treeExpansionChanged)
			.onSelectionChanged(this, &PathView::treeSelectionChanged)
			.onGetChildren(this, &PathView::getChildrenForTree);
		}
	}
	PathView::~PathView()
	{
	}
	void PathView::getChildrenForTree(std::shared_ptr<TreeItem> treeItem, std::vector<std::shared_ptr<TreeItem>>& outChildren)
	{
		//todo:添加排序的功能

		outChildren = treeItem->m_children;
	}
	void PathView::treeSelectionChanged(std::shared_ptr<TreeItem> treeItem, SelectInfo::Type selectInfo)
	{
		const std::vector<std::shared_ptr<TreeItem>> selectedItems = m_treeViewPtr->getSelectedItems();

		m_lastSelectedPaths.clear();
		for (int32_t itemIdx = 0; itemIdx < selectedItems.size(); ++itemIdx)
		{
			const std::shared_ptr<TreeItem> item = selectedItems[itemIdx];
			if (!item)
			{
				//所有的项都必须存在
				continue;
			}
			//保存选中的路径，用于过滤的时候，恢复过来
			m_lastSelectedPaths.insert(item->getItem().getPath());
		}

		if (m_itemSelectionChanged)
		{
			if (treeItem)
			{
				m_itemSelectionChanged(treeItem->getItem(), selectInfo);
			}
			else
			{
				m_itemSelectionChanged(ContentBrowserItem(), selectInfo);
			}
		}
	}
	void PathView::treeExpansionChanged(std::shared_ptr<TreeItem> treeItem, bool bIsExpanded)
	{
	}
	std::shared_ptr<ITableRow> PathView::generateTreeRow(std::shared_ptr<TreeItem> treeItem, const std::shared_ptr<TableViewBase>& ownerTable)
	{
		return WIDGET_NEW(TableRow<std::shared_ptr<TreeItem>>, ownerTable)
			.Content
			(
				WIDGET_NEW(TextBlockWidget)
			);
	}
}