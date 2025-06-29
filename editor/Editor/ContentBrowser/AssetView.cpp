#include <pch.h>

#include "AssetView.h"

#include <Core/UI/BoxPanel.h>
#include <Core/UI/Border.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Box.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/AssetDragDrop.h>
#include "AssetViewWidgets.h"
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/AssetManager/AssetManager.h>

namespace GuGu {
	void AssetView::init(const BuilderArguments& arguments)
	{
		m_tileViewThumbnailSize = 64;
		m_listViewThumbnailSize = 64;
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		//m_currentViewType = AssetViewType::Tile;
		m_onGetAssetContextMenu = arguments.monGetAssetContextMenu;
		m_onPathSelected = arguments.monPathSelected;
		m_backendFilter = arguments.minitialBackendFilter;
		m_currentViewType = arguments.minitialViewType;
		m_onAssetSelectionChanged = arguments.monAssetSelectionChanged;
		//create path view
		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox)	
		+ VerticalBox::Slot()
		.StretchHeight(1.0)
		(
			WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			.setVerticalAlignment(VerticalAlignment::Stretch)
			.setHorizontalAlignment(HorizontalAlignment::Stretch)
			(
				//显示资产文件和文件夹的 tile view 挂在这个容器下
				WIDGET_ASSIGN_NEW(Border, m_viewContainer)
				.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige4"))
			)			
		);
		createCurrentView();

		m_childWidget->m_childWidget = verticalBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}
	AssetView::~AssetView()
	{

	}
	void AssetView::createCurrentView()
	{
		m_tileView.reset();
		m_listView.reset();

		std::shared_ptr<Widget> newView = NullWidget::getNullWidget();
		switch (m_currentViewType)
		{
		case AssetViewType::Tile:
			m_tileView = createTileView();
			m_viewContainer->setContent(m_tileView);
			break;
		case AssetViewType::List:
			m_listView = createListView();
			m_viewContainer->setContent(m_listView);
			break;
		default:
			break;
		}
	}
	std::shared_ptr<AssetTileView> AssetView::createTileView()
	{
		return WIDGET_NEW(AssetTileView)
			.ListItemSource(&m_filteredAssetItems)
			.onGenerateTile(this, &AssetView::makeTileViewWidget)
			.itemHeight(92.0f)
			.onContextMenuOpening(this, &AssetView::onGetContextMenuContent)
			.onMouseButtonDoubleClick(this, &AssetView::onListMouseButtonDoubleClick)
			.onSelectionChanged(this, &AssetView::assetSelectionChanged);
	}

	std::shared_ptr<GuGu::AssetListView> AssetView::createListView()
	{
		return WIDGET_NEW(AssetListView)
			.ListItemSource(&m_filteredAssetItems)
			.onGenerateRow(this, &AssetView::makeListViewWidget)
			.itemHeight(92.0f)
			//.onContextMenuOpening(this, &AssetView::onGetContextMenuContent)
			.onMouseButtonDoubleClick(this, &AssetView::onListMouseButtonDoubleClick)
			.onSelectionChanged(this, &AssetView::assetSelectionChanged);
	}

	std::shared_ptr<ITableRow> AssetView::makeTileViewWidget(std::shared_ptr<AssetViewItem> assetItem, const std::shared_ptr<TableViewBase>& ownerTable)
	{
		if (!assetItem)
		{
			return WIDGET_NEW(TableRow<std::shared_ptr<AssetViewAsset>>, ownerTable);
		}

		if (assetItem->getType() == AssetItemType::Folder)
		{
			std::shared_ptr<AssetViewFolder> folderItem = std::static_pointer_cast<AssetViewFolder>(assetItem);
			std::shared_ptr<TableRow<std::shared_ptr<AssetViewItem>>> tableRowWidget;

			WIDGET_ASSIGN_NEW(TableRow<std::shared_ptr<AssetViewItem>>, tableRowWidget, ownerTable)
			.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.assetview"))
			.Content
			(
				WIDGET_NEW(GAssetTileItem)
				.assetItem(assetItem)
				.itemWidth(this, &AssetView::getTileViewItemWidth)
			);

			tableRowWidget->setToolTipText(folderItem->m_folderName);

			//std::shared_ptr<GAssetTileItem> item = WIDGET_NEW(GAssetTileItem)
			//	.assetItem(assetItem)
			//	.itemWidth(this, &AssetView::getTileViewItemWidth);
			//
			//tableRowWidget->setContent(item);
			return tableRowWidget;
		}
		else
		{
			std::shared_ptr<AssetViewAsset> assetItemAsAsset = std::static_pointer_cast<AssetViewAsset>(assetItem);

			if (assetItemAsAsset)
			{
				std::shared_ptr<TableRow<std::shared_ptr<AssetViewItem>>> tableRowWidget;

				WIDGET_ASSIGN_NEW(TableRow<std::shared_ptr<AssetViewItem>>, tableRowWidget, ownerTable)
				.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.assetview"))
				.Content
				(
					WIDGET_NEW(GAssetTileItem)
					.assetItem(assetItem)
					.itemWidth(this, &AssetView::getTileViewItemWidth)
				)
				.onDragDetected(this, &AssetView::onDraggingAssetItem);

				GuGuUtf8Str assetType = meta::Type::getType(assetItemAsAsset->m_data.m_assetTypeGuid).GetName();
				GuGuUtf8Str tooltip = "filePath:" + assetItemAsAsset->m_data.m_filePath + "\r\n" + "assetType:" + assetType;
				tableRowWidget->setToolTipText(tooltip);

				return tableRowWidget;		
			}

			return nullptr;
		}
	}

	std::shared_ptr<GuGu::ITableRow> AssetView::makeListViewWidget(std::shared_ptr<AssetViewItem> assetItem, const std::shared_ptr<TableViewBase>& ownerTable)
	{
		if (!assetItem)
		{
			return WIDGET_NEW(TableRow<std::shared_ptr<AssetViewAsset>>, ownerTable);
		}

		if (assetItem->getType() == AssetItemType::Folder)
		{
			std::shared_ptr<AssetViewFolder> folderItem = std::static_pointer_cast<AssetViewFolder>(assetItem);
			std::shared_ptr<TableRow<std::shared_ptr<AssetViewItem>>> tableRowWidget;

			WIDGET_ASSIGN_NEW(TableRow<std::shared_ptr<AssetViewItem>>, tableRowWidget, ownerTable)
				.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.assetview"))
				.Content
				(
					WIDGET_NEW(GAssetListItem)
					.assetItem(assetItem)
					.itemHeight(this, &AssetView::getListViewItemHeight)
				);

			tableRowWidget->setToolTipText(folderItem->m_folderName);

			//std::shared_ptr<GAssetTileItem> item = WIDGET_NEW(GAssetTileItem)
			//	.assetItem(assetItem)
			//	.itemWidth(this, &AssetView::getTileViewItemWidth);
			//
			//tableRowWidget->setContent(item);
			return tableRowWidget;
		}
		else
		{
			std::shared_ptr<AssetViewAsset> assetItemAsAsset = std::static_pointer_cast<AssetViewAsset>(assetItem);

			if (assetItemAsAsset)
			{
				std::shared_ptr<TableRow<std::shared_ptr<AssetViewItem>>> tableRowWidget;

				WIDGET_ASSIGN_NEW(TableRow<std::shared_ptr<AssetViewItem>>, tableRowWidget, ownerTable)
					.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.assetview"))
					.Content
					(
						WIDGET_NEW(GAssetListItem)
						.assetItem(assetItem)
						.itemHeight(this, &AssetView::getListViewItemHeight)
					)
					.onDragDetected(this, &AssetView::onDraggingAssetItem);

				GuGuUtf8Str assetType = meta::Type::getType(assetItemAsAsset->m_data.m_assetTypeGuid).GetName();
				GuGuUtf8Str tooltip = "filePath:" + assetItemAsAsset->m_data.m_filePath + "\r\n" + "assetType:" + assetType;
				tableRowWidget->setToolTipText(tooltip);

				return tableRowWidget;
			}

			return nullptr;
		}
	}

	void AssetView::setSourcesData(const GuGuUtf8Str& inSourcesData)
	{
		m_soucesData = inSourcesData;
		if (m_soucesData.len() > 0 && m_soucesData[0] == "/")
		{
			m_soucesData = m_soucesData.substr(1);
		}
		requestSlowFullListRefresh();
		clearSelection();//这个很重要，因为进入下一级文件夹的时候，可能还选中着上一级文件夹中的文件夹
		//右键菜单需要根据是否有选中文件夹，来弹出不同的菜单
	}
	void AssetView::refreshSourceItems()
	{
		m_filteredAssetItems.clear();

		const bool bShowAll = m_backendFilter.isEmpty();

		GuGuUtf8Str searchPath = m_soucesData + "/";
		searchPath = m_soucesData.substr(searchPath.findFirstOf("/") + 1);
		std::vector<AssetData> assetItems;
		if (bShowAll)
		{
			AssetManager::getAssetManager().getSubPaths(searchPath, [&](GuGuUtf8Str path, bool isDirectory) {
				if (!isDirectory)
				{
					//check is asset
					if (AssetManager::getAssetManager().isInAssetRegistry(path))
					{
						assetItems.push_back(AssetManager::getAssetManager().getAssetData(path));
					}
				}
			});
		}
		else
		{
			//recusive
			const bool bRecurse = shouldFilterRecursively();

			if (bRecurse)
			{
				//递归当前目录下的所有资产
				AssetManager::getAssetManager().traverseDirectoryAndFile(searchPath, [&](GuGuUtf8Str path, bool isDirectory) {
					if (!isDirectory)
					{
						//check is asset
						if (AssetManager::getAssetManager().isInAssetRegistry(path))
						{
							const AssetData& assetData = AssetManager::getAssetManager().getAssetData(path);
							if (meta::Type::getType(assetData.m_assetTypeGuid).GetName() == m_backendFilter.m_classNames[0]) //todo:先只筛选一个
							{
								assetItems.push_back(assetData);
							}
						}
					}
				});
			}
			else
			{
				AssetManager::getAssetManager().getSubPaths(searchPath, [&](GuGuUtf8Str path, bool isDirectory) {
					if (!isDirectory)
					{
						//check is asset
						if (AssetManager::getAssetManager().isInAssetRegistry(path))
						{
							assetItems.push_back(AssetManager::getAssetManager().getAssetData(path));
						}
					}
				});
			}
		}

		for (const AssetData& assetData : assetItems)
		{
			m_filteredAssetItems.push_back(std::make_shared<AssetViewAsset>(assetData));
		}
	}
	void AssetView::refreshFolders()
	{
		if (shouldFilterRecursively())
		{
			return;
		}
		std::vector<GuGuUtf8Str> assetPathsToShow;
		assetPathsToShow.push_back(m_soucesData);

		GuGuUtf8Str searchPath = m_soucesData + "/";
		searchPath = m_soucesData.substr(searchPath.findFirstOf("/") + 1);
		//获取当前文件夹下的子文件夹
		std::vector<GuGuUtf8Str> subPaths;
		AssetManager::getAssetManager().getSubPaths(searchPath, [&](GuGuUtf8Str path, bool isDirectory) {
			if (isDirectory)
			{
				subPaths.push_back(path);
			}
		});

		std::vector<GuGuUtf8Str> foldersToAdd;
		for (const GuGuUtf8Str& subPath : subPaths)
		{
			//m_folders.insert(subPath);//去重
			if (m_folders.find(subPath) == m_folders.end())
			{
				foldersToAdd.push_back(subPath);
			}
		}

		//if (foldersToAdd.size() > 0)
		//{
		for (const GuGuUtf8Str& folderPath : foldersToAdd)
		{
			m_filteredAssetItems.push_back(std::make_shared<AssetViewFolder>(folderPath));
		}

		refreshList();
		//}
	}
	void AssetView::refreshList()
	{
		switch (getCurrentViewType())
		{
		case AssetViewType::Tile:
			m_tileView->requestListRefresh();
			break;
		}
	}
	AssetViewType::Type AssetView::getCurrentViewType() const
	{
		return m_currentViewType;
	}
	void AssetView::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
	{
		if (m_bslowFullListRefreshRequested)
		{
			this->refreshSourceItems();
			this->refreshFolders();
			m_bslowFullListRefreshRequested = false;
		}	
	}
	void AssetView::requestSlowFullListRefresh()
	{
		m_bslowFullListRefreshRequested = true;
	}
	std::shared_ptr<Widget> AssetView::onGetContextMenuContent()
	{
		//todo:检查是否能够打开菜单栏

		const std::vector<GuGuUtf8Str> selectedFolders = getSelectedFolders();
		if (selectedFolders.size() > 0)
		{
			return m_onGetFolderContextMenu(selectedFolders);
		}
		else
		{
			return m_onGetAssetContextMenu(getSelectedAssets());//todo:传入选中的资产
		}

		return nullptr;
	}
	std::vector<GuGuUtf8Str> AssetView::getSelectedFolders() const
	{
		std::vector<std::shared_ptr<AssetViewItem>> selectedItems = getSelectedItems();
		std::vector<GuGuUtf8Str> selectedFolders;
		for (auto item : selectedItems)
		{
			if (item && item->getType() == AssetItemType::Folder)
			{
				selectedFolders.push_back(std::static_pointer_cast<AssetViewFolder>(item)->m_folderPath);
			}
		}
		return selectedFolders;
	}
	std::vector<std::shared_ptr<AssetViewItem>> AssetView::getSelectedItems() const
	{
		switch (getCurrentViewType())
		{
		case AssetViewType::Tile: return m_tileView->getSelectedItems();
		}
		return std::vector<std::shared_ptr<AssetViewItem>>();
	}
	GuGuUtf8Str AssetView::getSourcesData() const
	{
		return m_soucesData;
	}
	float AssetView::getTileViewItemWidth() const
	{
		return m_tileViewThumbnailSize;
	}

	float AssetView::getListViewItemHeight() const
	{
		return m_listViewThumbnailSize;
	}

	void AssetView::onListMouseButtonDoubleClick(std::shared_ptr<AssetViewItem> assetItem)
	{
		//todo:做更多检查
		if (assetItem->getType() == AssetItemType::Folder)
		{
			m_onPathSelected(std::static_pointer_cast<AssetViewFolder>(assetItem)->m_folderPath);
			return;
		}
	}

	void AssetView::clearSelection()
	{
		switch (getCurrentViewType())
		{
		case AssetViewType::Tile: m_tileView->clearSelection(); break;
		}
	}

	Reply AssetView::onDraggingAssetItem(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent)
	{
		std::vector<AssetData> draggedAssets;
		std::vector<GuGuUtf8Str> draggedAssetPaths;

		std::vector<AssetData> assetDataList = getSelectedAssets();
		for (const AssetData& assetData : assetDataList)
		{
			draggedAssets.push_back(assetData);
		}
		
		if (mouseEvent.isMouseButtonDown(Keys::LeftMouseButton))
		{
			if (draggedAssets.size() > 0)
			{
				return Reply::Handled().beginDragDrop(AssetDragDrop::New(draggedAssets, draggedAssetPaths));
			}
		}

		return Reply::Unhandled();
	}

	std::vector<AssetData> AssetView::getSelectedAssets() const
	{
		std::vector<std::shared_ptr<AssetViewItem>> selectedItems = getSelectedItems();
		std::vector<AssetData> selectedAssets;
		for (auto itemIt : selectedItems)
		{
			if (itemIt != nullptr && itemIt->getType() != AssetItemType::Folder)
			{
				selectedAssets.push_back(std::static_pointer_cast<AssetViewAsset>(itemIt)->m_data);
			}
		}
		return selectedAssets;
	}

	bool AssetView::shouldFilterRecursively()
	{
		if (!m_backendFilter.isEmpty())
		{
			return true;
		}
		return false;
	}

	void AssetView::assetSelectionChanged(std::shared_ptr<AssetViewItem> assetItem, SelectInfo::Type selectInfo)
	{
		if (assetItem && assetItem->getType() != AssetItemType::Folder)
		{
			if (m_onAssetSelectionChanged)
			{
				m_onAssetSelectionChanged(std::static_pointer_cast<AssetViewAsset>(assetItem)->m_data, selectInfo);
			}
		}
	}

}