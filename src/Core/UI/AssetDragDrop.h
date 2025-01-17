#pragma once

#include "DragDropOperation.h"
#include <Core/AssetManager/AssetData.h> //asset data

namespace GuGu {
	class AssetDragDrop : public DragDropOperation
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(AssetDragDrop, DragDropOperation)

		AssetDragDrop() {}
		
		virtual ~AssetDragDrop() {}

		const std::vector<AssetData>& getAssets() const
		{
			return m_assetData;
		}

		const std::vector<GuGuUtf8Str>& getAssetPaths() const
		{
			return m_assetPaths;
		}

		static std::shared_ptr<AssetDragDrop> New(const GuGuUtf8Str& inAssetPath);//创建窗口，创建 operation

		static std::shared_ptr<AssetDragDrop> New(std::vector<AssetData> inAssetData, std::vector<GuGuUtf8Str> inAssetPaths);
	private:
		std::vector<AssetData> m_assetData;

		std::vector<GuGuUtf8Str> m_assetPaths;

		int32_t m_thumbnailSize;
	};
}