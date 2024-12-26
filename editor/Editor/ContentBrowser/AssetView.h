#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

#include "ContentBrowserSingleton.h"

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
		};

		void init(const BuilderArguments& arguments);

		~AssetView();

		//创建显示资产和文件夹的view
		void createCurrentView();

		std::shared_ptr<AssetTileView> createTileView();

		std::shared_ptr<ITableRow> makeTileViewWidget(std::shared_ptr<AssetViewItem> assetItem, const std::shared_ptr<TableViewBase>& ownerTable);
	private:

		std::shared_ptr<Border> m_viewContainer;

		std::shared_ptr<AssetTileView> m_tileView;

		AssetViewType::Type m_currentViewType;

		std::vector<std::shared_ptr<AssetViewItem>> m_filteredAssetItems;
	};
}