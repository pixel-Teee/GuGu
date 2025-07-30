#include <pch.h>

#include "StaticMeshComponent.h"
#include <Core/AssetManager/AssetData.h>
#include <Core/AssetManager/AssetManager.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerStaticMeshComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::StaticMeshComponent");
		auto& type = db.types[id];
		meta::TypeInfo<StaticMeshComponent>::Register(id, type, true, "303B0EAC-B07E-42E1-B6E9-A56F00E75814");

		auto typeID = typeidof(StaticMeshComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<StaticMeshComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<StaticMeshComponent>();

			type.AddConstructor<StaticMeshComponent, false, false>({});

			type.AddConstructor<StaticMeshComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			//type.AddField<StaticMeshComponent, std::shared_ptr<AssetData>>("m_staticMeshAsset",
			//	(meta::FieldGetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature) & StaticMeshComponent::getStaticMeshAsset,
			//	(meta::FieldSetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature) & StaticMeshComponent::setStaticMeshAsset, {});
			//
			//type.AddField<StaticMeshComponent, std::weak_ptr<GameObject>>("m_owner",
			//	(meta::FieldGetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature) & StaticMeshComponent::getParentGameObject,
			//	(meta::FieldSetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature) & StaticMeshComponent::setParentGameObject, {});

			meta::TypeInfo<StaticMeshComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::StaticMeshComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<StaticMeshComponent>>::Register(id, type, false, "81D20567-8580-45DF-9DC9-785225EC85FF");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::StaticMeshComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<StaticMeshComponent>>::Register(id, type, false, "E07DC6B1-27A4-4875-B5FF-D49F82FAE211");
		}
		return true;
	}

	static bool registerStaticMeshComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(StaticMeshComponent).GetID()];
		
		type.AddField<StaticMeshComponent, std::shared_ptr<AssetData>>("m_staticMeshAsset",
			(meta::FieldGetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature) & StaticMeshComponent::getStaticMeshAsset,
			(meta::FieldSetter<StaticMeshComponent, std::shared_ptr<AssetData>, true>::Signature) & StaticMeshComponent::setStaticMeshAsset, {});

		type.AddField<StaticMeshComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature) & StaticMeshComponent::getParentGameObject,
			(meta::FieldSetter<StaticMeshComponent, std::weak_ptr<GameObject>&, true>::Signature) & StaticMeshComponent::setParentGameObject, {});
		
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(StaticMeshComponent)
		ADD_PRIORITY(AssetData)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerStaticMeshComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(StaticMeshComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerStaticMeshComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	StaticMeshComponent::StaticMeshComponent()
	{
		m_staticMeshAsset = std::make_shared<AssetData>();
		GuGuUtf8Str noFileExtensionsFileName = "defaultCube";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_staticMeshAsset = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GStaticMesh)));
	}
	StaticMeshComponent::~StaticMeshComponent()
	{
	}

	meta::Object* StaticMeshComponent::Clone(void) const
	{
		StaticMeshComponent* staticMeshComponent = new StaticMeshComponent();
		staticMeshComponent->m_staticMeshAsset = m_staticMeshAsset;
		return staticMeshComponent;
	}

	void StaticMeshComponent::Update(float fElapsedTimeSeconds)
	{
	}
	void StaticMeshComponent::setGStaticMesh(const GStaticMesh& gStaticMesh)
	{
		//copy static mesh
		m_staticMeshAsset->m_loadedResource = std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gStaticMesh.Clone()));
	}

	void StaticMeshComponent::setGStaticMesh(const std::shared_ptr<GStaticMesh>& gStaticMesh)
	{
		m_staticMeshAsset->m_loadedResource = gStaticMesh;
	}

	void StaticMeshComponent::setGStaticMesh(const std::shared_ptr<AssetData>& inAssetData)
	{
		m_staticMeshAsset = inAssetData;
	}

	std::shared_ptr<GStaticMesh> StaticMeshComponent::getStaticMesh() const
	{
		return std::static_pointer_cast<GStaticMesh>(m_staticMeshAsset->m_loadedResource);
	}
	std::shared_ptr<GStaticMesh> StaticMeshComponent::getStaticMesh()
	{
		return std::static_pointer_cast<GStaticMesh>(m_staticMeshAsset->m_loadedResource);
	}
	std::shared_ptr<AssetData> StaticMeshComponent::getStaticMeshAsset() const
	{
		return m_staticMeshAsset;
	}
	void StaticMeshComponent::setStaticMeshAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_staticMeshAsset = inAssetData;
	}
	meta::Type StaticMeshComponent::GetType() const
	{
		return typeof(StaticMeshComponent);
	}
}