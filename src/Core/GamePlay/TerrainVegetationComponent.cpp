#include <pch.h>

#include "TerrainVegetationComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include "TerrainComponent.h"

namespace GuGu {
	static bool registerGuGuTerrainVegetationComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::TerrainVegetationComponent");
		auto& type = db.types[id];
		meta::TypeInfo<TerrainVegetationComponent>::Register(id, type, true, "B12FC8AA-94BC-4FBD-845A-64B888070FDF");

		auto typeID = typeidof(TerrainVegetationComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TerrainVegetationComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<TerrainVegetationComponent>();

			type.AddConstructor<TerrainVegetationComponent, false, false>({});

			type.AddConstructor<TerrainVegetationComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<TerrainVegetationComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::TerrainVegetationComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<TerrainVegetationComponent>>::Register(id, type, false, "A87F8469-4A88-4C5F-ADBC-0DA752135A14");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::TerrainVegetationComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<TerrainVegetationComponent>>::Register(id, type, false, "5BB30CB2-1006-46F7-9AE9-B2DB1F9F9100");
		}
		return true;
	}
	static bool registerGuGuTerrainVegetationComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(TerrainVegetationComponent).GetID()];

		type.AddField<TerrainVegetationComponent, std::weak_ptr<TerrainComponent>>("m_terrainOwner",
			(meta::FieldGetter<TerrainVegetationComponent, std::weak_ptr<TerrainComponent>&, true>::Signature) & TerrainVegetationComponent::getTerrainOwner,
			(meta::FieldSetter<TerrainVegetationComponent, std::weak_ptr<TerrainComponent>&, true>::Signature) & TerrainVegetationComponent::setTerrainOwner, {});

		type.AddField<TerrainVegetationComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<TerrainVegetationComponent, std::weak_ptr<GameObject>&, true>::Signature) & TerrainVegetationComponent::getParentGameObject,
			(meta::FieldSetter<TerrainVegetationComponent, std::weak_ptr<GameObject>&, true>::Signature) & TerrainVegetationComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(TerrainVegetationComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuTerrainVegetationComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(TerrainVegetationComponent)
		ADD_PRIORITY_FIELDS(TerrainComponent)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuTerrainVegetationComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	TerrainVegetationComponent::TerrainVegetationComponent()
	{

	}

	TerrainVegetationComponent::~TerrainVegetationComponent()
	{

	}

	meta::Object* TerrainVegetationComponent::Clone(void) const
	{
		TerrainVegetationComponent* terrainVegetationComponent = new TerrainVegetationComponent();
		//terrainVegetationComponent->m_terrainOwner = m_terrainOwner;

		return terrainVegetationComponent;
	}

	void TerrainVegetationComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type TerrainVegetationComponent::GetType() const
	{
		return typeof(TerrainVegetationComponent);
	}

	void TerrainVegetationComponent::PostLoad()
	{
		//createTileData();
	}

	float TerrainVegetationComponent::getYOffset(math::float3 worldTranslation) const
	{
		//from terrain component height texture to get y offset
		if (m_terrainOwner.lock())
		{
			std::shared_ptr<TerrainComponent> terrainComponent = m_terrainOwner.lock();
			if (terrainComponent)
			{

				std::shared_ptr<GTexture> heightTexture = terrainComponent->getHeightTexture();

				//get uv
				math::float2 terrainSize = math::float2((float)terrainComponent->m_cols * (float)terrainComponent->m_tileSize, (float)terrainComponent->m_rows * (float)terrainComponent->m_tileSize);
				math::float2 terrainBeginXZ = math::float2(-(float)terrainComponent->m_terrainCols * terrainSize.x * 0.5f, -(float)terrainComponent->m_terrainRows * terrainSize.y * 0.5f);

				float x = worldTranslation.x;
				float z = worldTranslation.z;

				float h = -terrainBeginXZ.x * 2.0f;
				float v = -terrainBeginXZ.y * 2.0f;

				math::float2 uv = math::float2((x - terrainBeginXZ.x) / h, 1.0 - (z - terrainBeginXZ.y) / v);

				//GuGu_LOGD("Collision Terrain Tri UV:(%f, %f)", uv.x, uv.y);

				float textureWidth = heightTexture->m_width;
				float textureHeight = heightTexture->m_height;
				
				return heightTexture->getColor(uv.x * textureWidth, uv.y * textureHeight).x;
			}
		}
		return 0;
	}

	std::weak_ptr<TerrainComponent>& TerrainVegetationComponent::getTerrainOwner()
	{
		return m_terrainOwner;
	}

	void TerrainVegetationComponent::setTerrainOwner(const std::weak_ptr<TerrainComponent>& inTerrain)
	{
		m_terrainOwner = inTerrain;
	}

}