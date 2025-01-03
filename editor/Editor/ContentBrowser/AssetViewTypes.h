#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/FileSystem/FilePath.h>

namespace GuGu {
	namespace AssetItemType
	{
		enum Type
		{
			Normal,
			Folder,
			Creation,
			Duplication
		};
	}

	struct AssetViewItem
	{
		AssetViewItem() {}

		virtual ~AssetViewItem() {}

		virtual AssetItemType::Type getType() const = 0;

		//todo:增加更多的委托数据
	};

	struct AssetViewAsset : public AssetViewItem
	{
		AssetViewAsset() {}

		virtual ~AssetViewAsset() {}

		virtual AssetItemType::Type getType() const override
		{
			return AssetItemType::Normal;
		}
	};

	struct AssetViewFolder : public AssetViewItem
	{
		GuGuUtf8Str m_folderPath;

		GuGuUtf8Str m_folderName;

		AssetViewFolder(const GuGuUtf8Str& inPath)
			: m_folderPath(inPath)
		{
			GuGuUtf8Str splitPath = inPath;
			if (inPath[inPath.len() - 1] != "/")
				splitPath = inPath + "/";
			m_folderName = FilePath::getBaseFileName(splitPath);
		}

		virtual AssetItemType::Type getType() const override
		{
			return AssetItemType::Folder;
		}
	};
}