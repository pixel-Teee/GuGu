#include <pch.h>

#include "MaterialComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuMaterialComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::MaterialComponent");
		auto& type = db.types[id];
		meta::TypeInfo<MaterialComponent>::Register(id, type, true, "5B3EA494-8F58-4A28-A623-CCF822CDF3A1");

		auto typeID = typeidof(MaterialComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<MaterialComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<MaterialComponent>();

			type.AddConstructor<MaterialComponent, false, false>({});

			type.AddConstructor<MaterialComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<MaterialComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::MaterialComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<MaterialComponent>>::Register(id, type, false, "B502E20C-C5B2-4118-8B34-BE7284185CE4");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::MaterialComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<MaterialComponent>>::Register(id, type, false, "236A394A-0FD7-4339-95BA-C89B254474B9");
		}
		return true;
	}

	static bool registerGuGuMaterialComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(MaterialComponent).GetID()];
	

		type.AddField<MaterialComponent, Color>("m_albedo",
			(meta::FieldGetter<MaterialComponent, Color, false>::Signature) & MaterialComponent::m_albedo,
			(meta::FieldSetter<MaterialComponent, Color, false>::Signature) & MaterialComponent::m_albedo, {});

		type.AddField<MaterialComponent, float>("m_metallic",
			(meta::FieldGetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_metallic,
			(meta::FieldSetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_metallic, {});

		type.AddField<MaterialComponent, float>("m_roughness",
			(meta::FieldGetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_roughness,
			(meta::FieldSetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_roughness, {});

		type.AddField<MaterialComponent, float>("m_ao",
			(meta::FieldGetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_ao,
			(meta::FieldSetter<MaterialComponent, float, false>::Signature) & MaterialComponent::m_ao, {});

		type.AddField<MaterialComponent, std::shared_ptr<AssetData>>("m_albedoTexture",
			(meta::FieldGetter<MaterialComponent, std::shared_ptr<AssetData>, true>::Signature) & MaterialComponent::getAlbeoTextureAsset,
			(meta::FieldSetter<MaterialComponent, std::shared_ptr<AssetData>, true>::Signature) & MaterialComponent::setAlbeoTextureAsset, {});

		type.AddField<MaterialComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature) & MaterialComponent::getParentGameObject,
			(meta::FieldSetter<MaterialComponent, std::weak_ptr<GameObject>&, true>::Signature) & MaterialComponent::setParentGameObject, {});
		
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(MaterialComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuMaterialComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(MaterialComponent)
		ADD_PRIORITY_FIELDS(Color)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuMaterialComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	MaterialComponent::MaterialComponent()
	{
		m_albedo = Color(1.0f, 0.8f, 1.0f, 1.0f);
		m_metallic = 0.05f;
		m_roughness = 0.1f;
		m_ao = 0.0f;

		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_albedoTexture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));
	}
	MaterialComponent::~MaterialComponent()
	{
	}
	meta::Object* MaterialComponent::Clone(void) const
	{
		MaterialComponent* materialComponent = new MaterialComponent();
		materialComponent->m_albedo = m_albedo;
		materialComponent->m_metallic = m_metallic;
		materialComponent->m_roughness = m_roughness;
		materialComponent->m_ao = m_ao;
		materialComponent->m_albedoTexture = m_albedoTexture;
		return materialComponent;
	}
	void MaterialComponent::Update(float fElapsedTimeSeconds)
	{
	}
	meta::Type MaterialComponent::GetType() const
	{
		return meta::Type(typeof(MaterialComponent));
	}

	std::shared_ptr<GuGu::AssetData> MaterialComponent::getAlbeoTextureAsset() const
	{
		return m_albedoTexture;
	}

	void MaterialComponent::setAlbeoTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_albedoTexture = inAssetData;
	}

	std::shared_ptr<GTexture> MaterialComponent::getAlbedoTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_albedoTexture->m_loadedResource);
	}

}