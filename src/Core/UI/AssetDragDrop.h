#pragma once

#include "DecoratedDragDrop.h"
#include <Core/AssetManager/AssetData.h> //asset data

namespace GuGu {
	class AssetDragDrop : public DecoratedDragDropOp
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(AssetDragDrop, DecoratedDragDropOp)

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

		//作为生成窗口的内容
		virtual std::shared_ptr<Widget> getDefaultDecorator() const override;

		static std::shared_ptr<AssetDragDrop> New(const GuGuUtf8Str& inAssetPath);//创建窗口，创建 operation

		static std::shared_ptr<AssetDragDrop> New(std::vector<AssetData> inAssetData, std::vector<GuGuUtf8Str> inAssetPaths);
	private:
		std::vector<AssetData> m_assetData;

		std::vector<GuGuUtf8Str> m_assetPaths;

		int32_t m_thumbnailSize;
	};
}