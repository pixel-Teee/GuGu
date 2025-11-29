#include <pch.h>

#include "AssetData.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuAssetData()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::AssetData");
			auto& type = db.types[id];
			meta::TypeInfo<AssetData>::Register(id, type, true, "5A9DF858-72C3-4EC2-856D-3DF59457D874");

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });
		}
		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::AssetData>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<AssetData>>::Register(id, type, false, "21C647F2-BF00-4919-85EF-53D77FB3BC07");
		}
		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::AssetData>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<AssetData>>::Register(id, type, false, "3B42C324-06E4-49D3-AC0F-ADAE86A2DF39");
		}
		return true;
	}

	static bool registerGuGuAssetDataFields()
	{
		//nothing
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(AssetData)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuAssetData)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(AssetData)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuAssetDataFields)
	IMPLEMENT_INITIAL_FIELDS_END

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
		assetData->m_assetTypeGuid = m_assetTypeGuid;
		assetData->m_loadedResource = m_loadedResource;//资产还是唯一一份
		return assetData;
	}
	AssetData::AssetData(const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, const GuGuUtf8Str& assetTypeGuid)
		: m_filePath(filePath)
		, m_fileName(fileName)
		, m_assetTypeGuid(assetTypeGuid)
	{
	}

	GuGuUtf8Str AssetData::toString() const
	{
		char buf[128];
		std::sprintf(buf, "(m_filePath = %s, m_fileName = %s, m_assetType = %s)", m_filePath.getStr(), m_fileName.getStr(), m_assetTypeGuid.getGuid().getStr());
		return buf;
	}

}