#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

#include "ContentBrowserSingleton.h"
#include "ContentBrowserDelegates.h"
#include <Core/AssetManager/AssetData.h>
#include <Core/AssetManager/ARFilter.h>

namespace GuGu {
	using OnAssetSelectionChanged = std::function<void(const AssetData&, SelectInfo::Type)>;
	class ITableRow;
	class Border;
	class AssetTileView;
	class AssetListView;
	struct AssetViewItem;
	class TableViewBase;
	//显示过滤的资产的 widget
	class AssetView : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<AssetView>
		{
			BuilderArguments()
			: minitialViewType(AssetViewType::Type::Tile)
			{}

			~BuilderArguments() = default;

			UI_EVENT(OnGetAssetContextMenu, onGetAssetContextMenu)

			UI_EVENT(OnPathSelected, onPathSelected)

			UI_EVENT(OnAssetSelectionChanged, onAssetSelectionChanged)

			ARGUMENT_VALUE(ARFilter, initialBackendFilter)

			ARGUMENT_VALUE(AssetViewType::Type, initialViewType)
		};

		void init(const BuilderArguments& arguments);

		~AssetView();

		//创建显示资产和文件夹的view
		void createCurrentView();

		std::shared_ptr<AssetTileView> createTileView();

		std::shared_ptr<AssetListView> createListView();

		std::shared_ptr<ITableRow> makeTileViewWidget(std::shared_ptr<AssetViewItem> assetItem, const std::shared_ptr<TableViewBase>& ownerTable);

		std::shared_ptr<ITableRow> makeListViewWidget(std::shared_ptr<AssetViewItem> assetItem, const std::shared_ptr<TableViewBase>& ownerTable);

		void assetRenameCommitted(const std::shared_ptr<AssetViewItem>& inItem, const GuGuUtf8Str& newName, const math::box2& messageAnchor, const TextCommit::Type commitType);

		void setSourcesData(const GuGuUtf8Str& inSourcesData);

		//重新生成资产
		void refreshSourceItems();

		//重新生成文件夹，如果我们需要显示它们
		void refreshFolders();

		void refreshList();

		AssetViewType::Type getCurrentViewType() const;

		virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

		void requestSlowFullListRefresh();

		std::shared_ptr<Widget> onGetContextMenuContent();

		std::vector<GuGuUtf8Str> getSelectedFolders() const;

		std::vector<std::shared_ptr<AssetViewItem>> getSelectedItems() const;

		GuGuUtf8Str getSourcesData() const;

		float getTileViewItemWidth() const;

		float getListViewItemHeight() const;

		void onListMouseButtonDoubleClick(std::shared_ptr<AssetViewItem> assetItem);

		void clearSelection();

		Reply onDraggingAssetItem(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent);

		std::vector<AssetData> getSelectedAssets() const;

		bool shouldFilterRecursively();

		void assetSelectionChanged(std::shared_ptr<AssetViewItem> assetItem, SelectInfo::Type selectInfo);

		Reply OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent);

	private:

		GuGuUtf8Str m_soucesData;//当前所处于的文件夹

		//当前view中显示的文件夹项
		std::set<GuGuUtf8Str> m_folders;

		std::shared_ptr<Border> m_viewContainer;

		std::shared_ptr<AssetTileView> m_tileView;

		std::shared_ptr<AssetListView> m_listView;

		AssetViewType::Type m_currentViewType;

		std::vector<std::shared_ptr<AssetViewItem>> m_filteredAssetItems;

		bool m_bslowFullListRefreshRequested = false;

		OnGetFolderContextMenu m_onGetFolderContextMenu;

		OnGetAssetContextMenu m_onGetAssetContextMenu;

		int32_t m_tileViewThumbnailSize;

		int32_t m_listViewThumbnailSize;

		OnPathSelected m_onPathSelected;

		ARFilter m_backendFilter;

		OnAssetSelectionChanged m_onAssetSelectionChanged;
	};
}