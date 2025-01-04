#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

#include "ContentBrowserSingleton.h"
#include "ContentBrowserDelegates.h"

namespace GuGu {
	class ITableRow;
	class Border;
	class AssetTileView;
	class AssetViewItem;
	class TableViewBase;
	//显示过滤的资产的 widget
	class AssetView : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<AssetView>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			UI_EVENT(OnGetAssetContextMenu, onGetAssetContextMenu)
		};

		void init(const BuilderArguments& arguments);

		~AssetView();

		//创建显示资产和文件夹的view
		void createCurrentView();

		std::shared_ptr<AssetTileView> createTileView();

		std::shared_ptr<ITableRow> makeTileViewWidget(std::shared_ptr<AssetViewItem> assetItem, const std::shared_ptr<TableViewBase>& ownerTable);

		void setSourcesData(const GuGuUtf8Str& inSourcesData);

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
	private:

		GuGuUtf8Str m_soucesData;//当前所处于的文件夹

		//当前view中显示的文件夹项
		std::set<GuGuUtf8Str> m_folders;

		std::shared_ptr<Border> m_viewContainer;

		std::shared_ptr<AssetTileView> m_tileView;

		AssetViewType::Type m_currentViewType;

		std::vector<std::shared_ptr<AssetViewItem>> m_filteredAssetItems;

		bool m_bslowFullListRefreshRequested = false;

		OnGetAssetContextMenu m_onGetAssetContextMenu;
	};
}