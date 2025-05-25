#include <pch.h>

#include "WaterComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuWaterComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::WaterComponent");
		auto& type = db.types[id];
		meta::TypeInfo<WaterComponent>::Register(id, type, true, "4BEAE916-760A-4142-9321-5DC622A4B640");

		auto typeID = typeidof(WaterComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<WaterComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<WaterComponent>();

			type.AddConstructor<WaterComponent, false, false>({});

			type.AddConstructor<WaterComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<WaterComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::WaterComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<WaterComponent>>::Register(id, type, false, "5942C0D7-9173-443E-98CC-0DBA19D3C9BA");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::WaterComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<WaterComponent>>::Register(id, type, false, "E40553A2-248D-49DD-A284-FC35991219C1");
		}
		return true;
	}
	static bool registerGuGuWaterComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::WaterComponent");
		auto& type = db.types[typeof(WaterComponent).GetID()];

		type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_mixedColorTexture",
			(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_mixedColorTexture,
			(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_mixedColorTexture, {});

		type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_colorTexture",
			(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_colorTexture,
			(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_colorTexture, {});

		type.AddField<WaterComponent, std::shared_ptr<AssetData>>("m_dirTexture",
			(meta::FieldGetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_dirTexture,
			(meta::FieldSetter<WaterComponent, std::shared_ptr<AssetData>, false>::Signature) & WaterComponent::m_dirTexture, {});

		type.AddField<WaterComponent, uint32_t>("m_waterRows",
			(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterRows,
			(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterRows, {});

		type.AddField<WaterComponent, uint32_t>("m_waterCols",
			(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterCols,
			(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_waterCols, {});

		type.AddField<WaterComponent, uint32_t>("m_rows",
			(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_rows,
			(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_rows, {});

		type.AddField<WaterComponent, uint32_t>("m_cols",
			(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_cols,
			(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_cols, {});

		type.AddField<WaterComponent, uint32_t>("m_tileSize",
			(meta::FieldGetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_tileSize,
			(meta::FieldSetter<WaterComponent, uint32_t, false>::Signature) & WaterComponent::m_tileSize, {});

		type.AddField<WaterComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<WaterComponent, std::weak_ptr<GameObject>&, true>::Signature) & WaterComponent::getParentGameObject,
			(meta::FieldSetter<WaterComponent, std::weak_ptr<GameObject>&, true>::Signature) & WaterComponent::setParentGameObject, {});
		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(WaterComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuWaterComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(WaterComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuWaterComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END
	WaterComponent::WaterComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		std::shared_ptr<AssetData> whiteTexture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		m_dirTexture = whiteTexture;
		m_mixedColorTexture = whiteTexture;
		m_colorTexture = whiteTexture;
		m_waterCols = 10;
		m_waterRows = 10;
		m_rows = 20;
		m_cols = 20;
		m_tileSize = 2;
		createTileData();
	}

	WaterComponent::~WaterComponent()
	{

	}

	meta::Object* WaterComponent::Clone(void) const
	{
		WaterComponent* waterComponent = new WaterComponent();
		waterComponent->m_dirTexture = m_dirTexture;
		waterComponent->m_mixedColorTexture = m_mixedColorTexture;
		waterComponent->m_colorTexture = m_colorTexture;
		waterComponent->m_waterCols = m_waterCols;
		waterComponent->m_waterRows = m_waterRows;
		waterComponent->m_rows = m_rows;
		waterComponent->m_cols = m_cols;
		waterComponent->m_tileSize = m_tileSize;

		return waterComponent;
	}

	void WaterComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type WaterComponent::GetType() const
	{
		return typeof(WaterComponent);
	}

	void WaterComponent::PostLoad()
	{

	}

	void WaterComponent::createTileData()
	{
		int32_t rows = m_rows + 1;
		int32_t cols = m_cols + 1;
		uint32_t faceCount = rows * cols * 2;

		float halfWidth = 0.5f * m_tileSize * cols;
		float halfDepth = 0.5f * m_tileSize * rows;

		float dx = m_tileSize;
		float dz = m_tileSize;

		for (uint32_t i = 0; i < rows; ++i)
		{
			float z = -halfDepth + i * dz;
			for (uint32_t j = 0; j < cols; ++j)
			{
				float x = -halfWidth + j * dx;
				m_vertexData.push_back(math::float3(x, 0, z));//no y
			}
		}

		m_indexData.reserve(faceCount * 3);

		uint32_t k = 0;
		for (uint32_t i = 0; i < rows - 1; ++i)
		{
			for (uint32_t j = 0; j < cols - 1; ++j)
			{
				m_indexData.push_back((i + 1) * cols + j);
				m_indexData.push_back(i * cols + j + 1);
				m_indexData.push_back(i * cols + j);

				m_indexData.push_back((i + 1) * cols + j + 1);
				m_indexData.push_back(i * cols + j + 1);
				m_indexData.push_back((i + 1) * cols + j);
			}
		}
	}

	std::shared_ptr<GTexture> WaterComponent::getDirTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_dirTexture->m_loadedResource);
	}

	std::shared_ptr<GTexture> WaterComponent::getMixedColorTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_mixedColorTexture->m_loadedResource);
	}

	std::shared_ptr<GTexture> WaterComponent::getColorTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_colorTexture->m_loadedResource);
	}

}