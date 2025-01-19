#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class AssetView;
	class PathView;
	class WindowWidget;
	struct AssetData;
	class ContentBrowser : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<ContentBrowser>
		{
			BuilderArguments(){}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<WindowWidget> inParentWindow);

		~ContentBrowser();

		void pathSelected(const GuGuUtf8Str& folderPath);

		void folderEntered(const GuGuUtf8Str& folderPath);

		std::shared_ptr<Widget> onGetAssetContextMenu(const std::vector<AssetData>& selectedAssets);//todo:修复这个，增加选中资产
	private:

		std::shared_ptr<AssetView> m_assetView;

		std::shared_ptr<PathView> m_pathView;

		std::shared_ptr<WindowWidget> m_parentWindow;
	};
}