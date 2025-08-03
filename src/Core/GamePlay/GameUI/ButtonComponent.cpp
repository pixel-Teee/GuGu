#include <pch.h>

#include "ButtonComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/AssetManager/AssetData.h>
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include "UITransformComponent.h"
#include "UIComponent.h"
#include "UIDrawInfo.h"
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuButtonComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::ButtonComponent");
		auto& type = db.types[id];
		meta::TypeInfo<ButtonComponent>::Register(id, type, true, "5DBE330E-E391-4F9F-8506-9B0B16A2642D");

		auto typeID = typeidof(ButtonComponent);

		//array constructor
		type.SetArrayConstructor<ButtonComponent>();

		type.AddConstructor<ButtonComponent, false, false>({});

		type.AddConstructor<ButtonComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(UIComponent) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::ButtonComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<ButtonComponent>>::Register(id, type, false, "C8AB4808-12C4-4336-B9BC-A7336A2FDEAE");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::ButtonComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<ButtonComponent>>::Register(id, type, false, "F82AF1C7-5B86-4999-8A07-245EE7B36FC5");
		}

		return true;
	}

	static bool registerGuGuButtonComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(ButtonComponent).GetID()];

		type.AddField<ButtonComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<ButtonComponent, std::weak_ptr<GameObject>&, true>::Signature) & ButtonComponent::getParentGameObject,
			(meta::FieldSetter<ButtonComponent, std::weak_ptr<GameObject>&, true>::Signature) & ButtonComponent::setParentGameObject, {});

		type.AddField<ButtonComponent, std::shared_ptr<AssetData>>("m_buttonImage",
			(meta::FieldGetter<ButtonComponent, std::shared_ptr<AssetData>, true>::Signature) & ButtonComponent::getTextureAsset,
			(meta::FieldSetter<ButtonComponent, std::shared_ptr<AssetData>, true>::Signature) & ButtonComponent::setTextureAsset, {});

		type.AddField<ButtonComponent, Color>("m_normalColor",
			(meta::FieldGetter<ButtonComponent, Color&, true>::Signature) & ButtonComponent::getNormalColor,
			(meta::FieldSetter<ButtonComponent, Color, true>::Signature) & ButtonComponent::setNormalColor, {});

		type.AddField<ButtonComponent, Color>("m_hoveredColor",
			(meta::FieldGetter<ButtonComponent, Color&, true>::Signature) & ButtonComponent::getHoveredColor,
			(meta::FieldSetter<ButtonComponent, Color, true>::Signature) & ButtonComponent::setHoveredColor, {});

		type.AddField<ButtonComponent, Color>("m_pressedColor",
			(meta::FieldGetter<ButtonComponent, Color&, true>::Signature) & ButtonComponent::getPressedColor,
			(meta::FieldSetter<ButtonComponent, Color, true>::Signature) & ButtonComponent::setPressedColor, {});

		type.AddField<ButtonComponent, Color>("m_disabledColor",
			(meta::FieldGetter<ButtonComponent, Color&, true>::Signature) & ButtonComponent::getDisabledColor,
			(meta::FieldSetter<ButtonComponent, Color, true>::Signature) & ButtonComponent::setDisabledColor, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(ButtonComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuButtonComponent)
		IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(ButtonComponent)
		ADD_PRIORITY(Color)
		ADD_PRIORITY(AssetData)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuButtonComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	ButtonComponent::ButtonComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_buttonImage = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		m_normalColor = Color(0.18f, 0.64f, 0.30f, 1.0f);
		m_hoveredColor = Color(0.172f, 0.592f, 0.294f, 1.0f);
		m_pressedColor = Color(0.16f, 0.55f, 0.274f, 1.0f);
		m_disabledColor = Color(0.58f, 0.82f, 0.63f, 1.0f);

		m_bIsHovered = false;
		m_bIsPressed = false;
	}

	ButtonComponent::~ButtonComponent()
	{

	}

	meta::Object* ButtonComponent::Clone(void) const
	{
		ButtonComponent* buttonComponent = new ButtonComponent();
		buttonComponent->m_buttonImage = this->m_buttonImage;
		buttonComponent->m_normalColor = this->m_normalColor;
		buttonComponent->m_pressedColor = this->m_pressedColor;
		buttonComponent->m_disabledColor = this->m_disabledColor;
		return buttonComponent;
	}

	void ButtonComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type ButtonComponent::GetType() const
	{
		return typeof(ButtonComponent);
	}

	void ButtonComponent::setTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_buttonImage = inAssetData;
	}

	std::shared_ptr<GTexture> ButtonComponent::getTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_buttonImage->m_loadedResource);
	}

	std::shared_ptr<AssetData> ButtonComponent::getTextureAsset() const
	{
		return m_buttonImage;
	}

	Color ButtonComponent::getNormalColor() const
	{
		return m_normalColor;
	}

	Color& ButtonComponent::getNormalColor()
	{
		return m_normalColor;
	}

	void ButtonComponent::setNormalColor(Color inColor)
	{
		m_normalColor = inColor;
	}

	Color ButtonComponent::getHoveredColor() const
	{
		return m_hoveredColor;
	}

	Color& ButtonComponent::getHoveredColor()
	{
		return m_hoveredColor;
	}

	void ButtonComponent::setHoveredColor(Color inColor)
	{
		m_hoveredColor = inColor;
	}

	Color ButtonComponent::getPressedColor() const
	{
		return m_pressedColor;
	}

	Color& ButtonComponent::getPressedColor()
	{
		return m_pressedColor;
	}

	void ButtonComponent::setPressedColor(Color inColor)
	{
		m_pressedColor = inColor;
	}

	Color ButtonComponent::getDisabledColor() const
	{
		return m_disabledColor;
	}

	Color& ButtonComponent::getDisabledColor()
	{
		return m_disabledColor;
	}

	void ButtonComponent::setDisabledColor(Color inColor)
	{
		m_disabledColor = inColor;
	}

	std::shared_ptr<UIDrawInfo> ButtonComponent::generateUIDrawInformation(bool bFlip /*= false*/)
	{
		std::shared_ptr<UIDrawInfo> drawInfo = std::make_shared<UIDrawInfo>();
		drawInfo->m_texture = getTexture();

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

			Color currentColor;
			if (m_bIsPressed)
			{
				currentColor = m_pressedColor;
			}
			else if (m_bIsHovered)
			{
				currentColor = m_hoveredColor;
			}
			else
			{
				currentColor = m_normalColor;
			}

			//vertex generate
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(0, 1), math::float3(absolutePos.x, absolutePos.y, 0), currentColor));
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(1, 1), math::float3(absolutePos.x + localSize.x, absolutePos.y, 0), currentColor));
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(0, 0), math::float3(absolutePos.x, absolutePos.y + localSize.y, 0), currentColor));
			drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(1, 0), math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y, 0), currentColor));

			//index generate
			drawInfo->m_uiIndices.push_back(0);
			drawInfo->m_uiIndices.push_back(2);
			drawInfo->m_uiIndices.push_back(1);

			drawInfo->m_uiIndices.push_back(1);
			drawInfo->m_uiIndices.push_back(2);
			drawInfo->m_uiIndices.push_back(3);

			drawInfo->m_zorder = uiTransformComponent->getZOrder();
		}


		return drawInfo;
	}

	math::float2 ButtonComponent::getDesiredSize() const
	{
		std::shared_ptr<GTexture> texture = getTexture();
		if (texture)
		{
			return math::float2(texture->m_width, texture->m_height);
		}
		return math::float2(0, 0);
	}

	void ButtonComponent::onPointerDown(UIPointerData pointerData)
	{
		//GuGu_LOGD("trigger me %s", "button");
	}

	void ButtonComponent::onPointerUp(UIPointerData pointerData)
	{

	}

}