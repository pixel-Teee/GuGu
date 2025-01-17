#include <pch.h>

#include "AssetDragDrop.h"

namespace GuGu {
    std::shared_ptr<AssetDragDrop> AssetDragDrop::New(const GuGuUtf8Str& inAssetPath)
    {
        std::vector<GuGuUtf8Str> assetPaths;
        assetPaths.push_back(inAssetPath);
        return New(std::vector<AssetData>(), assetPaths);
    }
    std::shared_ptr<AssetDragDrop> AssetDragDrop::New(std::vector<AssetData> inAssetData, std::vector<GuGuUtf8Str> inAssetPaths)
    {
        std::shared_ptr<AssetDragDrop> operation = std::make_shared<AssetDragDrop>();

        operation->m_thumbnailSize = 64;

        operation->m_assetData = inAssetData;
        operation->m_assetPaths = inAssetPaths;

        //operation->construct();
        return operation;
    }
}