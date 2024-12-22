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
	void PathView::setSelectedPaths(const std::vector<GuGuUtf8Str>& paths)
	{
		if (!m_treeViewPtr)
		{
			return;
		}

		//todo: 清除 search box

		m_lastSelectedPaths.clear();
		m_treeViewPtr->clearSelection();

		for (int32_t pathIndex = 0; pathIndex < paths.size(); ++pathIndex)
		{
			const GuGuUtf8Str& path = paths[pathIndex];

			std::vector<GuGuUtf8Str> pathItemList;
			{			
				path.stringSplit("/", pathItemList);//todo:空白字符串不加入数组
			}

			//------
			if (pathItemList.size())
			{
				std::vector<std::shared_ptr<TreeItem>> treeItems;

				for (int32_t rootItemIndex = 0; rootItemIndex < m_treeRootItems.size(); ++rootItemIndex)
				{
					if (m_treeRootItems[rootItemIndex]->getItem().getItemName() == pathItemList[0])
					{
						//找到路径中第一个item
						treeItems.push_back(m_treeRootItems[rootItemIndex]);
						break;
					}
				}

				if (treeItems.size() > 0)
				{
					for (int32_t pathItemIndex = 1; pathItemIndex < pathItemList.size(); ++pathItemIndex)
					{
						const GuGuUtf8Str pathItemName = pathItemList[pathItemIndex];
						//把 tree items 和提供的路径所匹配的儿子找出来
						const std::shared_ptr<TreeItem> childItem = treeItems.back()->getChild(pathItemName);

						if (childItem)
						{
							treeItems.push_back(childItem);
						}
						else
						{
							break;
						}
					}

					//展开所有的树文件夹，不包含最后一个
					for (int32_t itemIndex = 0; itemIndex < treeItems.size() - 1; ++itemIndex)
					{
						m_treeViewPtr->setItemExpansion(treeItems[itemIndex], true);
					}

					m_lastSelectedPaths.insert(treeItems.back()->getItem().getPath());
					m_treeViewPtr->setItemSelection(treeItems.back(), true);
					m_treeViewPtr->requestScrollIntoView(treeItems.back());
				}
				else
				{

				}
			}
			else
			{

			}
		}
	}
}