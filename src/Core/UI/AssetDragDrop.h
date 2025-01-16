#pragma once

#include "DragDropOperation.h"
#include <Core/AssetManager/AssetData.h> //asset data

namespace GuGu {
	class AssetDragDropOperation : public DragDropOperation
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(AssetDragDropOperation, DragDropOperation)

		AssetDragDropOperation() {}
		
		virtual ~AssetDragDropOperation() {}

		const std::vector<AssetData>& getAssets() const
		{
			return m_assetData;
		}

		const std::vector<GuGuUtf8Str>& getAssetPaths() const
		{
			return m_assetPaths;
		}
	private:
		std::vector<AssetData> m_assetData;

		std::vector<GuGuUtf8Str> m_assetPaths;
	};
}