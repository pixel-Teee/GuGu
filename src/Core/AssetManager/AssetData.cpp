#include <pch.h>

#include "AssetData.h"

namespace GuGu {
	void AssetData::Update(float fElapsedTimeSeconds)
	{
	}

	void AssetData::PostLoad()
	{

	}

	meta::Type AssetData::GetType() const
	{
		return typeof(AssetData);
	}
	meta::Object* AssetData::Clone(void) const
	{
		AssetData* assetData = new AssetData();
		assetData->m_filePath = m_filePath;
		assetData->m_fileName = m_fileName;
		assetData->m_assetType = m_assetType;
		assetData->m_loadedResource = m_loadedResource;//todo:可能要修复
		return assetData;
	}
	AssetData::AssetData(const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, meta::Type assetType)
		: m_filePath(filePath)
		, m_fileName(fileName)
		, m_assetType(assetType)
	{
	}
}