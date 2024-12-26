#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

#include "ContentBrowserSingleton.h"

namespace GuGu {
	class Border;
	class AssetTileView;
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
	private:

		std::shared_ptr<Border> m_viewContainer;

		std::shared_ptr<AssetTileView> m_tileView;

		AssetViewType::Type m_currentViewType;
	};
}