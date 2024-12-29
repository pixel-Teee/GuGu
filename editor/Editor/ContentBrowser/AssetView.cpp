#include <pch.h>

#include "AssetView.h"

#include <Core/UI/BoxPanel.h>
#include <Core/UI/Border.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Box.h>
#include <Core/UI/Overlay.h>

#include "AssetViewWidgets.h"
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/AssetManager/AssetManager.h>

namespace GuGu {
	void AssetView::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_currentViewType = AssetViewType::Tile;
		//create path view
		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox)	
		+ VerticalBox::Slot()
		.setPadding(Padding(14.0f, 14.0f, 14.0f, 14.0f))
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
	}
	AssetView::~AssetView()
	{

	}
	void AssetView::createCurrentView()
	{
		m_tileView.reset();

		std::shared_ptr<Widget> newView = NullWidget::getNullWidget();
		switch (m_currentViewType)
		{
		case AssetViewType::Tile:
			m_tileView = createTileView();
			break;
		default:
			break;
		}
		m_viewContainer->setContent(m_tileView);
	}
	std::shared_ptr<AssetTileView> AssetView::createTileView()
	{
		return WIDGET_NEW(AssetTileView)
				.ListItemSource(&m_filteredAssetItems)
				.onGenerateTile(this, &AssetView::makeTileViewWidget)
				.itemHeight(40.0f);
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
			.Content(			
				WIDGET_NEW(TextBlockWidget)
				.text(folderItem->m_folderName)	
				.textColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
			);

			return tableRowWidget;
		}
		else
		{
			std::shared_ptr<AssetViewAsset> assetItemAsAsset = std::static_pointer_cast<AssetViewAsset>(assetItem);

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
	}
	void AssetView::refreshFolders()
	{
		if (m_bslowFullListRefreshRequested)
		{
			m_filteredAssetItems.clear();

			std::vector<GuGuUtf8Str> assetPathsToShow;
			assetPathsToShow.push_back(m_soucesData);

			GuGuUtf8Str searchPath = m_soucesData + "/";
			searchPath = m_soucesData.substr(searchPath.findFirstOf("/"));
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

			m_bslowFullListRefreshRequested = false;
		}	
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
		m_filteredAssetItems.clear();
		this->refreshFolders();
	}
	void AssetView::requestSlowFullListRefresh()
	{
		m_bslowFullListRefreshRequested = true;
	}
}