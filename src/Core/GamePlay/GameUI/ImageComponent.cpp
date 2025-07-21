#include <pch.h>

#include "ImageComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include "UITransformComponent.h"
#include "UIDrawInfo.h"

namespace GuGu {
	static bool registerGuGuImageComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::ImageComponent");
		auto& type = db.types[id];
		meta::TypeInfo<ImageComponent>::Register(id, type, true, "D97F59C0-35AB-4436-904E-B2BA72046DE1");

		auto typeID = typeidof(ImageComponent);

		//array constructor
		type.SetArrayConstructor<ImageComponent>();

		type.AddConstructor<ImageComponent, false, false>({});

		type.AddConstructor<ImageComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(UIComponent) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::ImageComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<ImageComponent>>::Register(id, type, false, "ACAE011B-89C8-40C2-A467-F90BD32BF41E");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::ImageComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<ImageComponent>>::Register(id, type, false, "28F8A3F5-4D7A-4435-9785-B4FE109AED95");
		}

		return true;
	}

	static bool registerGuGuImageComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(ImageComponent).GetID()];

		type.AddField<ImageComponent, std::shared_ptr<AssetData>>("m_texture",
			(meta::FieldGetter<ImageComponent, std::shared_ptr<AssetData>, true>::Signature) & ImageComponent::getTextureAsset,
			(meta::FieldSetter<ImageComponent, std::shared_ptr<AssetData>, true>::Signature) & ImageComponent::setTextureAsset, {});

		type.AddField<ImageComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<ImageComponent, std::weak_ptr<GameObject>&, true>::Signature) & ImageComponent::getParentGameObject,
			(meta::FieldSetter<ImageComponent, std::weak_ptr<GameObject>&, true>::Signature) & ImageComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(ImageComponent)
		ADD_PRIORITY(UIComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuImageComponent)
		IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(ImageComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuImageComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	ImageComponent::ImageComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_texture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));
	}

	ImageComponent::~ImageComponent()
	{

	}

	meta::Object* ImageComponent::Clone(void) const
	{
		ImageComponent* imageComponent = new ImageComponent();
		imageComponent->m_texture = m_texture;
		return imageComponent;
	}

	void ImageComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type ImageComponent::GetType() const
	{
		return typeof(ImageComponent);
	}

	void ImageComponent::setTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_texture = inAssetData;
	}

	std::shared_ptr<GTexture> ImageComponent::getTextureAsset() const
	{
		return std::static_pointer_cast<GTexture>(m_texture->m_loadedResource);
	}

	std::shared_ptr<UIDrawInfo> ImageComponent::generateUIDrawInformation()
	{
		std::shared_ptr<UIDrawInfo> drawInfo = std::make_shared<UIDrawInfo>();
		drawInfo->m_uiTextureHandle = getTextureAsset()->m_texture;

		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner)
		{
			//------
			// 0---1
			// |   |
			// 2---3
			//------
			std::shared_ptr<UITransformComponent> uiTransformComponent = owner->getComponent<UITransformComponent>();

			//get global transform
			math::affine3 worldTransform = uiTransformComponent->GetLocalToWorldTransformFloat();

			math::float3 absolutePos;
			math::quat absoluteQuat;
			math::float3 absoluteScale;
			
			math::decomposeAffine(worldTransform, &absolutePos, &absoluteQuat, &absoluteScale);

			math::float2 localSize = uiTransformComponent->getLocalSize();

			//vertex generate
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(0, 1), math::float3(absolutePos.x, absolutePos.y, absolutePos.z * 0.001f), Color(1.0f, 1.0f, 1.0f, 1.0f)));
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(1, 1), math::float3(absolutePos.x + localSize.x, absolutePos.y, absolutePos.z * 0.001f), Color(1.0f, 1.0f, 1.0f, 1.0f)));
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(0, 0), math::float3(absolutePos.x, absolutePos.y + localSize.y, absolutePos.z * 0.001f), Color(1.0f, 1.0f, 1.0f, 1.0f)));
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(1, 0), math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y, absolutePos.z * 0.001f), Color(1.0f, 1.0f, 1.0f, 1.0f)));

			//index generate
			drawInfo->m_uiIndices.push_back(0);
			drawInfo->m_uiIndices.push_back(1);
			drawInfo->m_uiIndices.push_back(2);

			drawInfo->m_uiIndices.push_back(1);
			drawInfo->m_uiIndices.push_back(3);
			drawInfo->m_uiIndices.push_back(2);
		}
		

		return drawInfo;
	}

}