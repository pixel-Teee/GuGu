#include <pch.h>

#include "PathView.h"
#include "TreeItem.h"
#include "SourcesViewWidgets.h"


#include <Core/AssetManager/AssetManager.h>

#include <Editor/StyleSet/EditorStyleSet.h>
#include <Core/UI/Box.h>

namespace GuGu {
	void PathView::init(const BuilderArguments& arguments)
	{
		m_onPathSelected = arguments.monPathSelected;
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

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = WIDGET_NEW(BoxWidget)
		.padding(Padding(10.0f, 10.0f, 10.0f, 10.0f))
		.Content
		(
			WIDGET_NEW(Border)
			.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige4"))
			.Content
			(
				m_treeViewPtr
			)	
		);
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		//填充路径
		populate();
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
			m_lastSelectedPaths.insert(item->m_folderPath);

			if (m_onPathSelected)
			{
				if (treeItem)
				{
					m_onPathSelected(treeItem->m_folderPath);
				}
				else
				{
					m_onPathSelected("");
				}
			}
		}


	}
	void PathView::treeExpansionChanged(std::shared_ptr<TreeItem> treeItem, bool bIsExpanded)
	{
	}
	std::shared_ptr<ITableRow> PathView::generateTreeRow(std::shared_ptr<TreeItem> treeItem, const std::shared_ptr<TableViewBase>& ownerTable)
	{
		return WIDGET_NEW(TableRow<std::shared_ptr<TreeItem>>, ownerTable)
			.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.beige"))
			.Content
			(
				//WIDGET_NEW(TextBlockWidget)
				//.text(treeItem->m_folderName)
				//.textColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
				WIDGET_NEW(AssetTreeItem)
				.treeItem(treeItem)
				.isItemExpanded(this, &PathView::isTreeItemExpanded, treeItem)
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

			//------pathItemList是一个将路径按/分割的每个目录
			if (pathItemList.size())
			{
				std::vector<std::shared_ptr<TreeItem>> treeItems;

				for (int32_t rootItemIndex = 0; rootItemIndex < m_treeRootItems.size(); ++rootItemIndex)
				{
					if (m_treeRootItems[rootItemIndex]->m_folderPath == pathItemList[0])
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

					m_lastSelectedPaths.insert(treeItems.back()->m_folderPath);
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
	void PathView::populate()
	{
		//AssetManager::getAssetManager().traverseDirectoryAndFile([](GuGuUtf8Str path, bool isDirectory) {
		//			
		//});
		GuGuUtf8Str rootPath = AssetManager::getAssetManager().getRootPath();

		addPath(rootPath);//content

		std::vector<GuGuUtf8Str> allPathLists;
		AssetManager::getAssetManager().traverseDirectoryAndFile([&](GuGuUtf8Str path, bool isDirectory) {
			if (isDirectory)
			{
				allPathLists.push_back(path);
			}
		});

		for (int32_t pathIndex = 0; pathIndex < allPathLists.size(); ++pathIndex)
		{
			const GuGuUtf8Str& path = allPathLists[pathIndex];
			std::shared_ptr<TreeItem> item = addPath(path);
			if (item)
			{
				//todo:实现这里的逻辑
			}
		}

		m_treeViewPtr->requestTreeRefresh();
	}
	std::shared_ptr<TreeItem> PathView::addPath(const GuGuUtf8Str& path, bool bUserNamed)
	{
		if (!m_treeViewPtr)
		{
			return std::make_shared<TreeItem>();
		}

		std::vector<GuGuUtf8Str> pathItemList;
		path.stringSplit("/", pathItemList);

		if (pathItemList.size())
		{
			std::shared_ptr<TreeItem> currentItem;

			//找到 pathItemList 中的根
			for (int32_t rootItemIndex = 0; rootItemIndex < m_treeRootItems.size(); ++rootItemIndex)
			{
				if (m_treeRootItems[rootItemIndex]->m_folderName == pathItemList[0])
				{
					currentItem = m_treeRootItems[rootItemIndex];
					break;
				}
			}

			if (!currentItem)
			{
				currentItem = addRootItem(pathItemList[0]);
			}

			if (currentItem)
			{
				for (int32_t pathItemIndex = 1; pathItemIndex < pathItemList.size(); ++pathItemIndex)
				{
					const GuGuUtf8Str& pathItemName = pathItemList[pathItemIndex];
					std::shared_ptr<TreeItem> childItem = currentItem->getChild(pathItemName);

					if (!childItem)
					{
						const GuGuUtf8Str folderName = pathItemName;
						const GuGuUtf8Str folderPath = currentItem->m_folderPath + "/" + pathItemName;

						if (!bUserNamed)
						{
							//todo:实现这里
						}

						childItem = std::make_shared<TreeItem>(folderName, folderPath, currentItem, bUserNamed);
						currentItem->m_children.push_back(childItem);
						//todo:add TreeItem requestSortChildren
						m_treeViewPtr->requestTreeRefresh();

					}
					else
					{
						childItem->m_folderPath = currentItem->m_folderPath + "/" + pathItemName;
					}

					currentItem = childItem;
				}

				//todo:增加 naming folder 逻辑
			}

			return currentItem;
		}

		return std::make_shared<TreeItem>();
	}
	std::shared_ptr<TreeItem> PathView::addRootItem(const GuGuUtf8Str& inFolderName)
	{
		for (int32_t rootItemIndex = 0; rootItemIndex < m_treeRootItems.size(); ++rootItemIndex)
		{
			if (m_treeRootItems[rootItemIndex]->m_folderName == inFolderName)
			{
				return m_treeRootItems[rootItemIndex];
			}
		}

		std::shared_ptr<TreeItem> newItem = nullptr;

		newItem = std::make_shared<TreeItem>(inFolderName, "/" + inFolderName, nullptr);
		m_treeRootItems.push_back(newItem);
		m_treeViewPtr->requestTreeRefresh();

		return newItem;
	}
	bool PathView::isTreeItemExpanded(std::shared_ptr<TreeItem> treeItem) const
	{
		return m_treeViewPtr->isItemExpanded(treeItem);
	}
}