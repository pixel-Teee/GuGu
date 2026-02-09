#include <pch.h>

#include "TerrainComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/Collision/Collision3D.h>

namespace GuGu {
	static bool registerGuGuTerrainComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::TerrainComponent");
		auto& type = db.types[id];
		meta::TypeInfo<TerrainComponent>::Register(id, type, true, "D813E5D9-B950-4941-8577-4C9C78B9C9D1");

		auto typeID = typeidof(TerrainComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TerrainComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<TerrainComponent>();

			type.AddConstructor<TerrainComponent, false, false>({});

			type.AddConstructor<TerrainComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<TerrainComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::TerrainComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<TerrainComponent>>::Register(id, type, false, "39C665D1-522D-4321-83D4-C1946DC0D84F");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::TerrainComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<TerrainComponent>>::Register(id, type, false, "0C93471F-0542-4DE4-9A38-9D6E7E977C4F");
		}
		return true;
	}
	static bool registerGuGuTerrainComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(TerrainComponent).GetID()];
		
		type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_heightTexture",
			(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_heightTexture,
			(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_heightTexture, {});

		type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture1",
			(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture1,
			(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture1, {});

		type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture2",
			(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture2,
			(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture2, {});

		type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture3",
			(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture3,
			(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture3, {});

		type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_terrainTexture4",
			(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture4,
			(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_terrainTexture4, {});

		type.AddField<TerrainComponent, std::shared_ptr<AssetData>>("m_blendTexture",
			(meta::FieldGetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_blendTexture,
			(meta::FieldSetter<TerrainComponent, std::shared_ptr<AssetData>, false>::Signature) & TerrainComponent::m_blendTexture, {});

		type.AddField<TerrainComponent, uint32_t>("m_terrainRows",
			(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainRows,
			(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainRows, {});

		type.AddField<TerrainComponent, uint32_t>("m_terrainCols",
			(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainCols,
			(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_terrainCols, {});

		type.AddField<TerrainComponent, uint32_t>("m_rows",
			(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_rows,
			(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_rows, {});

		type.AddField<TerrainComponent, uint32_t>("m_cols",
			(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_cols,
			(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_cols, {});

		type.AddField<TerrainComponent, uint32_t>("m_tileSize",
			(meta::FieldGetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_tileSize,
			(meta::FieldSetter<TerrainComponent, uint32_t, false>::Signature) & TerrainComponent::m_tileSize, {});

		type.AddField<TerrainComponent, float>("m_heightScale",
			(meta::FieldGetter<TerrainComponent, float, true>::Signature) & TerrainComponent::getHeightScale,
			(meta::FieldSetter<TerrainComponent, float, true>::Signature) & TerrainComponent::setHeightScale, {});

		type.AddField<TerrainComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<TerrainComponent, std::weak_ptr<GameObject>&, true>::Signature) & TerrainComponent::getParentGameObject,
			(meta::FieldSetter<TerrainComponent, std::weak_ptr<GameObject>&, true>::Signature) & TerrainComponent::setParentGameObject, {});
		
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(TerrainComponent)
		ADD_PRIORITY(AssetData)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuTerrainComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(TerrainComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuTerrainComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	TerrainComponent::TerrainComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		std::shared_ptr<AssetData> whiteTexture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		m_heightTexture = whiteTexture;
		m_terrainTexture1 = whiteTexture;
		m_terrainTexture2 = whiteTexture;
		m_terrainTexture3 = whiteTexture;
		m_terrainTexture4 = whiteTexture;
		m_blendTexture = whiteTexture;
		m_terrainCols = 10;
		m_terrainRows = 10;
		m_rows = 20;
		m_cols = 20;
		m_tileSize = 2;
		m_heightScale = 1.0f;
		createTileData();
	}

	TerrainComponent::~TerrainComponent()
	{

	}

	meta::Object* TerrainComponent::Clone(void) const
	{
		TerrainComponent* terrainComponent = new TerrainComponent();
		terrainComponent->m_heightTexture   = m_heightTexture;
		terrainComponent->m_terrainTexture1 = m_terrainTexture1;
		terrainComponent->m_terrainTexture2 = m_terrainTexture2;
		terrainComponent->m_terrainTexture3 = m_terrainTexture3;
		terrainComponent->m_terrainTexture4 = m_terrainTexture4;
		terrainComponent->m_blendTexture    = m_blendTexture;
		terrainComponent->m_terrainCols		= m_terrainCols;
		terrainComponent->m_terrainRows		= m_terrainRows;
		terrainComponent->m_rows			= m_rows;
		terrainComponent->m_cols			= m_cols;
		terrainComponent->m_tileSize		= m_tileSize;
		terrainComponent->m_heightScale		= m_heightScale;

		return terrainComponent;
	}

	void TerrainComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type TerrainComponent::GetType() const
	{
		return typeof(TerrainComponent);
	}

	void TerrainComponent::PostLoad()
	{
		createTileData();
	}

	void TerrainComponent::createTileData()
	{
		m_vertexData.clear();
		int32_t rows = m_rows + 1;//generate vertex
		int32_t cols = m_cols + 1;
		uint32_t faceCount = rows * cols * 2;

		float halfWidth = 0.5f * m_tileSize * m_rows;
		float halfDepth = 0.5f * m_tileSize * m_cols;

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

		m_indexData.clear();
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
		//10.0f 是 height scale
		//m_objectSpaceBounds = dm::box3(m_vertexData.size(), m_vertexData.data());
		math::float3 oneBlockSize = math::float3((float)m_cols * (float)m_tileSize, 0, (float)m_rows * (float)m_tileSize);
		math::float3 beginXZ = math::float3((float)m_terrainCols * oneBlockSize.x * 0.5f, 0, (float)m_terrainRows * oneBlockSize.z * 0.5f);
		m_objectSpaceBounds = dm::box3(math::float3(-beginXZ.x, -m_heightScale, -beginXZ.z), math::float3(beginXZ.x, m_heightScale, beginXZ.z));

		m_indexBufferHandle = m_vertexBufferHandle = nullptr;
	}

	math::box3 TerrainComponent::getObjectSpaceBounds() const
	{
		return m_objectSpaceBounds;
	}

	float TerrainComponent::getHeightScale() const
	{
		return m_heightScale;
	}

	void TerrainComponent::setHeightScale(float inHeightScale)
	{
		if (m_heightScale != inHeightScale)
		{
			std::shared_ptr<GTexture> heightTexture = getHeightTexture();
			if (heightTexture)
			{
				int32_t textureWidth = heightTexture->m_width;
				int32_t textureHeight = heightTexture->m_height;
				for (int32_t i = 0; i < textureWidth; ++i)
				{
					for (int32_t j = 0; j < textureHeight; ++j)
					{
						math::float3 originalColor = heightTexture->getColor(i, j);
						math::float3 newColor = originalColor * (m_heightScale / inHeightScale);
						math::float4 result = math::float4(newColor.x, newColor.x, newColor.x, newColor.x);
						heightTexture->setColor(i, j, result);
					}
				}
			}

			heightTexture->m_isDirty = true;

			m_heightScale = inHeightScale;
		}
	}

	std::shared_ptr<GTexture> TerrainComponent::getHeightTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_heightTexture->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture1() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture1->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture2() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture2->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture3() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture3->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture4() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture4->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getBlendTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_blendTexture->m_loadedResource);
	}

	std::vector<uint8_t> TerrainComponent::getSampleArray() const
	{
		std::vector<uint8_t> sampleCountArray;
		math::float2 terrainSize = math::float2((float)m_cols * (float)m_tileSize, (float)m_rows * (float)m_tileSize);
		math::float2 terrainBeginXZ = math::float2(-(float)m_terrainCols * terrainSize.x * 0.5f, -(float)m_terrainRows * terrainSize.y * 0.5f);

		std::shared_ptr<GTexture> heightTexture = getHeightTexture();

		float heightScale = m_heightScale;
		const auto& positions = m_vertexData;
		const auto& indices = m_indexData;
		uint32_t triCount = indices.size() / 3;
		for (uint32_t i = 0; i < triCount; i = i + 2)
		{
			uint32_t i0 = indices[i * 3 + 0];
			uint32_t i1 = indices[i * 3 + 1];
			uint32_t i2 = indices[i * 3 + 2];

			math::float3 position0 = positions[i0];
			math::float3 position1 = positions[i1];
			math::float3 position2 = positions[i2];

			for (uint32_t i = 0; i < m_terrainRows; ++i)
			{
				for (uint32_t j = 0; j < m_terrainCols; ++j)
				{
					math::float2 offsetXZ = math::float2(terrainBeginXZ.x + j * terrainSize.x + terrainSize.x / 2.0f, terrainBeginXZ.y + i * terrainSize.y + terrainSize.y / 2.0f);

					math::float3 position3;
					math::float3 position4;
					math::float3 position5;
					Collision3D::getTerrainPosition(position0, position1, position2, position3, position4, position5, offsetXZ, terrainBeginXZ, heightTexture, heightScale);
					sampleCountArray.push_back(position3.y * 255.0);//height
				}
			}
		}
		return sampleCountArray;
	}

}