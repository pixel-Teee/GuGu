#include <pch.h>

#include "CubeComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuCubeComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::CubeComponent");
		auto& type = db.types[id];
		meta::TypeInfo<CubeComponent>::Register(id, type, true, "D4766ACC-0624-46C0-8E61-84E1D52EEDDD");

		auto typeID = typeidof(CubeComponent);

		//array constructor
		type.SetArrayConstructor<CubeComponent>();

		type.AddConstructor<CubeComponent, false, false>({});

		type.AddConstructor<CubeComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(Component) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::CubeComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<CubeComponent>>::Register(id, type, false, "6DAFEB0D-F09E-4EDE-923E-285D1EEAB803");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::CubeComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<CubeComponent>>::Register(id, type, false, "76C07A92-E1F6-409C-9E41-D021D8D0E3CF");
		}

		return true;
	}

	static bool registerGuGuCubeComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(CubeComponent).GetID()];

		type.AddField<CubeComponent, std::shared_ptr<AssetData>>("m_left",
			(meta::FieldGetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::getLeftTextureAsset,
			(meta::FieldSetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::setLeftTextureAsset, {});

		type.AddField<CubeComponent, std::shared_ptr<AssetData>>("m_right",
			(meta::FieldGetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::getLeftTextureAsset,
			(meta::FieldSetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::setLeftTextureAsset, {});

		type.AddField<CubeComponent, std::shared_ptr<AssetData>>("m_front",
			(meta::FieldGetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::getLeftTextureAsset,
			(meta::FieldSetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::setLeftTextureAsset, {});

		type.AddField<CubeComponent, std::shared_ptr<AssetData>>("m_back",
			(meta::FieldGetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::getLeftTextureAsset,
			(meta::FieldSetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::setLeftTextureAsset, {});

		type.AddField<CubeComponent, std::shared_ptr<AssetData>>("m_top",
			(meta::FieldGetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::getLeftTextureAsset,
			(meta::FieldSetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::setLeftTextureAsset, {});

		type.AddField<CubeComponent, std::shared_ptr<AssetData>>("m_bottom",
			(meta::FieldGetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::getLeftTextureAsset,
			(meta::FieldSetter<CubeComponent, std::shared_ptr<AssetData>, true>::Signature) & CubeComponent::setLeftTextureAsset, {});
		//
		//type.AddField<CubeComponent, Color>("m_color",
		//	(meta::FieldGetter<CubeComponent, Color, false>::Signature) & CubeComponent::m_color,
		//	(meta::FieldSetter<CubeComponent, Color, false>::Signature) & CubeComponent::m_color, {});
		//
		//type.AddField<CubeComponent, std::weak_ptr<GameObject>>("m_owner",
		//	(meta::FieldGetter<CubeComponent, std::weak_ptr<GameObject>&, true>::Signature) & CubeComponent::getParentGameObject,
		//	(meta::FieldSetter<CubeComponent, std::weak_ptr<GameObject>&, true>::Signature) & CubeComponent::setParentGameObject, {});

		//type.AddMethod("setTextureAsset", &CubeComponent::setTextureAsset, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(CubeComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCubeComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(CubeComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		//ADD_PRIORITY_FIELDS(Color)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCubeComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	CubeComponent::CubeComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";

		m_left = 
		m_right = 
		m_front = 
		m_back = 
		m_top = 
		m_bottom = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		//m_color = Color(0.5f, 1.0f, 1.0f, 1.0f);
		m_bDirty = true;
	}

	CubeComponent::~CubeComponent()
	{

	}

	meta::Object* CubeComponent::Clone(void) const
	{
		CubeComponent* cubeComponent = new CubeComponent();
		//CubeComponent->m_texture = m_texture;
		//CubeComponent->m_color = m_color;
		cubeComponent->m_left = m_left;
		cubeComponent->m_right = m_right;
		cubeComponent->m_front = m_front;
		cubeComponent->m_back = m_back;
		cubeComponent->m_top = m_top;
		cubeComponent->m_bottom = m_bottom;
		return cubeComponent;
	}

	void CubeComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type CubeComponent::GetType() const
	{
		return typeof(CubeComponent);
	}

	void CubeComponent::setLeftTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_left = inAssetData;
	}

	std::shared_ptr<GTexture> CubeComponent::getLeftTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_left->m_loadedResource);
	}

	std::shared_ptr<AssetData> CubeComponent::getLeftTextureAsset() const
	{
		return m_left;
	}

	void CubeComponent::setRightTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_right = inAssetData;
	}

	std::shared_ptr<GTexture> CubeComponent::getRightTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_right->m_loadedResource);
	}

	std::shared_ptr<AssetData> CubeComponent::getRightTextureAsset() const
	{
		return m_right;
	}

	void CubeComponent::setFrontTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_front = inAssetData;
	}

	std::shared_ptr<GTexture> CubeComponent::getFrontTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_front->m_loadedResource);
	}

	std::shared_ptr<AssetData> CubeComponent::getFrontTextureAsset() const
	{
		return m_front;
	}

	void CubeComponent::setBackTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_back = inAssetData;
	}

	std::shared_ptr<GTexture> CubeComponent::getBackTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_back->m_loadedResource);
	}

	std::shared_ptr<AssetData> CubeComponent::getBackTextureAsset() const
	{
		return m_back;
	}

	void CubeComponent::setTopTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_top = inAssetData;
	}

	std::shared_ptr<GTexture> CubeComponent::getTopTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_top->m_loadedResource);
	}

	std::shared_ptr<AssetData> CubeComponent::getTopTextureAsset() const
	{
		return m_top;
	}

	void CubeComponent::setBottomTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_bottom = inAssetData;
	}

	std::shared_ptr<GTexture> CubeComponent::getBottomTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_bottom->m_loadedResource);
	}

	std::shared_ptr<AssetData> CubeComponent::getBottomTextureAsset() const
	{
		return m_bottom;
	}

	void CubeComponent::setDirty(bool isDirty)
	{
		m_bDirty = isDirty;
	}

	bool CubeComponent::isDirty() const
	{
		return m_bDirty;
	}

	//void CubeComponent::setTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	//{
	//	m_texture = inAssetData;
	//}
	//
	//std::shared_ptr<GTexture> CubeComponent::getTexture() const
	//{
	//	return std::static_pointer_cast<GTexture>(m_texture->m_loadedResource);
	//}
	//
	//std::shared_ptr<AssetData> CubeComponent::getTextureAsset() const
	//{
	//	return m_texture;
	//}

}