#include <pch.h>

#include "SpriteComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuSpriteComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::SpriteComponent");
		auto& type = db.types[id];
		meta::TypeInfo<SpriteComponent>::Register(id, type, true, "FE44932A-09D1-4A32-9A52-076085FAC11A");

		auto typeID = typeidof(SpriteComponent);

		//array constructor
		type.SetArrayConstructor<SpriteComponent>();

		type.AddConstructor<SpriteComponent, false, false>({});

		type.AddConstructor<SpriteComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(Component) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::SpriteComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<SpriteComponent>>::Register(id, type, false, "F1736E95-8F83-4FF8-B323-6AE98547F3A4");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::SpriteComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<SpriteComponent>>::Register(id, type, false, "4ECD6FEE-174A-4E84-AEAB-EC40C2535D98");
		}

		return true;
	}

	static bool registerGuGuSpriteComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(SpriteComponent).GetID()];

		type.AddField<SpriteComponent, std::shared_ptr<AssetData>>("m_texture",
			(meta::FieldGetter<SpriteComponent, std::shared_ptr<AssetData>, true>::Signature) & SpriteComponent::getTextureAsset,
			(meta::FieldSetter<SpriteComponent, std::shared_ptr<AssetData>, true>::Signature) & SpriteComponent::setTextureAsset, {});

		type.AddField<SpriteComponent, Color>("m_color",
			(meta::FieldGetter<SpriteComponent, Color, false>::Signature) & SpriteComponent::m_color,
			(meta::FieldSetter<SpriteComponent, Color, false>::Signature) & SpriteComponent::m_color, {});

		type.AddField<SpriteComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<SpriteComponent, std::weak_ptr<GameObject>&, true>::Signature) & SpriteComponent::getParentGameObject,
			(meta::FieldSetter<SpriteComponent, std::weak_ptr<GameObject>&, true>::Signature) & SpriteComponent::setParentGameObject, {});

		type.AddMethod("setTextureAsset", &SpriteComponent::setTextureAsset, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(SpriteComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuSpriteComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(SpriteComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_PRIORITY_FIELDS(Color)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuSpriteComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	SpriteComponent::SpriteComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_texture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		m_color = Color(0.5f, 1.0f, 1.0f, 1.0f);
	}

	SpriteComponent::~SpriteComponent()
	{

	}

	meta::Object* SpriteComponent::Clone(void) const
	{
		SpriteComponent* spriteComponent = new SpriteComponent();
		spriteComponent->m_texture = m_texture;
		spriteComponent->m_color = m_color;
		return spriteComponent;
	}

	void SpriteComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type SpriteComponent::GetType() const
	{
		return typeof(SpriteComponent);
	}

	void SpriteComponent::setTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_texture = inAssetData;
	}

	std::shared_ptr<GTexture> SpriteComponent::getTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_texture->m_loadedResource);
	}

	std::shared_ptr<AssetData> SpriteComponent::getTextureAsset() const
	{
		return m_texture;
	}

}