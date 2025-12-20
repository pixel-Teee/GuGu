#pragma once

#include "AssetViewTypes.h"
#include "SourcesViewWidgets.h"

#include <Core/UI/TileView.h>

namespace GuGu {

	using OnRenameCommit = std::function<void(const std::shared_ptr<AssetViewItem>&, //asset item
		const GuGuUtf8Str&, //original name
		const math::box2&, //message anchor
		TextCommit::Type)>; //commit type
	//在 asset tile view 中的一个项
	//处理拖动
	class GAssetViewItem : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<GAssetViewItem>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			//数据源
			ARGUMENT_VALUE(std::shared_ptr<AssetViewItem>, assetItem)

			UI_EVENT(IsSelected, isSelected)

			UI_EVENT(OnRenameCommit, onRenameCommit)
		};

		void init(const BuilderArguments& arguments);

		void handleNameCommitted(const GuGuUtf8Str& newText, TextCommit::Type commitInfo);

		std::shared_ptr<AssetViewItem> m_assetItem;

		bool isFolder() const;

		GuGuUtf8Str getNameText() const;

		IsSelected m_isSelected;//call back

		OnRenameCommit m_onRenameCommit;//rename commit

		WidgetGeometry m_lastGeometry;
	};

	class GAssetTileItem : public GAssetViewItem
	{
	public:
		struct BuilderArguments : public Arguments<GAssetTileItem>
		{
			BuilderArguments() 
				: mitemWidth(16)
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(float, itemWidth)

			ARGUMENT_VALUE(std::shared_ptr<AssetViewItem>, assetItem)

			UI_EVENT(IsSelected, isSelected)

			UI_EVENT(OnRenameCommit, onRenameCommit)
		};

		void init(const BuilderArguments& arguments);

		OptionalSize getThumbnailBoxSize() const;

		Attribute<float> m_itemWidth;
	};

	class GAssetListItem : public GAssetViewItem
	{
	public:
		struct BuilderArguments : public Arguments<GAssetListItem>
		{
			BuilderArguments()
				: mitemHeight(16)
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(float, itemHeight)

			ARGUMENT_VALUE(std::shared_ptr<AssetViewItem>, assetItem)
		};

		void init(const BuilderArguments& arguments);

		OptionalSize getThumbnailBoxSize() const;

		Attribute<float> m_itemHeight;
	};

	class AssetTileView : public TileView<std::shared_ptr<AssetViewItem>>
	{
	public:
		virtual bool supportsKeyboardFocus() const { return true; }
	};

	class AssetListView : public ListView<std::shared_ptr<AssetViewItem>>
	{
	public:
		virtual bool supportsKeyboardFocus() const { return true; }
	};
}