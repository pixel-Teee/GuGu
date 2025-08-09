#include <pch.h>

#include "TextComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include "UITransformComponent.h"
#include "UIDrawInfo.h"

namespace GuGu {
	static bool registerGuGuTextComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::TextComponent");
		auto& type = db.types[id];
		meta::TypeInfo<TextComponent>::Register(id, type, true, "1E4E898F-886E-4623-B6DF-BF5911548428");

		auto typeID = typeidof(TextComponent);

		//array constructor
		type.SetArrayConstructor<TextComponent>();

		type.AddConstructor<TextComponent, false, false>({});

		type.AddConstructor<TextComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(UIComponent) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::TextComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<TextComponent>>::Register(id, type, false, "C058CD30-105C-40E4-912B-6C31DC129644");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::TextComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<TextComponent>>::Register(id, type, false, "09089768-C7A5-43F7-9600-6473A149D5D0");
		}

		return true;
	}

	static bool registerGuGuTextComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(TextComponent).GetID()];

		type.AddField<TextComponent, Color>("m_color",
			(meta::FieldGetter<TextComponent, Color&, true>::Signature) & TextComponent::getColor,
			(meta::FieldSetter<TextComponent, Color, true>::Signature) & TextComponent::setColor, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(TextComponent)
		ADD_PRIORITY(UIComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuTextComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(TextComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_PRIORITY_FIELDS(Color)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuTextComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	TextComponent::TextComponent()
	{

		m_color = Color(0.5f, 1.0f, 1.0f, 1.0f);
	}

	TextComponent::~TextComponent()
	{

	}

	meta::Object* TextComponent::Clone(void) const
	{
		TextComponent* textComponent = new TextComponent();
		textComponent->m_color = m_color;
		return textComponent;
	}

	void TextComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type TextComponent::GetType() const
	{
		return typeof(TextComponent);
	}

	std::shared_ptr<UIDrawInfo> TextComponent::generateUIDrawInformation(bool bFlip)
	{
		std::shared_ptr<UIDrawInfo> drawInfo = std::make_shared<UIDrawInfo>();
		//drawInfo->m_texture = getTexture();
		//
		//std::shared_ptr<GameObject> owner = m_owner.lock();
		//if (owner)
		//{
		//	//------
		//	// 0---1
		//	// |   |
		//	// 2---3
		//	//------
		//	std::shared_ptr<UITransformComponent> uiTransformComponent = owner->getComponent<UITransformComponent>();
		//
		//	//get global transform
		//	math::affine3 worldTransform = uiTransformComponent->GetLocalToWorldTransformFloat();
		//
		//	math::float3 absolutePos;
		//	math::quat absoluteQuat;
		//	math::float3 absoluteScale;
		//
		//	math::decomposeAffine(worldTransform, &absolutePos, &absoluteQuat, &absoluteScale);
		//
		//	math::float2 localSize = uiTransformComponent->getLocalSize();
		//
		//	//vertex generate
		//	drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(0, 1), math::float3(absolutePos.x, absolutePos.y, 0), m_color));
		//	drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(1, 1), math::float3(absolutePos.x + localSize.x, absolutePos.y, 0), m_color));
		//	drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(0, 0), math::float3(absolutePos.x, absolutePos.y + localSize.y, 0), m_color));
		//	drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(1, 0), math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y, 0), m_color));
		//
		//	//index generate
		//	drawInfo->m_uiIndices.push_back(0);
		//	drawInfo->m_uiIndices.push_back(2);
		//	drawInfo->m_uiIndices.push_back(1);
		//
		//	drawInfo->m_uiIndices.push_back(1);
		//	drawInfo->m_uiIndices.push_back(2);
		//	drawInfo->m_uiIndices.push_back(3);
		//
		//	drawInfo->m_zorder = uiTransformComponent->getZOrder();
		//}
		//
		//
		return drawInfo;
	}

	math::float2 TextComponent::getDesiredSize() const
	{
		//nothing to do
		return math::float2(0, 0);
	}

	Color TextComponent::getColor() const
	{
		return m_color;
	}

	Color& TextComponent::getColor()
	{
		return m_color;
	}

	void TextComponent::setColor(Color& inColor)
	{
		m_color = inColor;
	}

}