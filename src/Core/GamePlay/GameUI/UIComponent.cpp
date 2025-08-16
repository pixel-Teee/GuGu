#include <pch.h>

#include "UIComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

#include "UITransformComponent.h"
#include "UIDrawInfo.h"

namespace GuGu {
	static bool registerGuGuUIComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIComponent");
		auto& type = db.types[id];
		meta::TypeInfo<UIComponent>::Register(id, type, true, "F196C5E2-E9CA-4128-8E95-B7455934BAB3");

		auto typeID = typeidof(UIComponent);

		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<UIComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<UIComponent>();

			type.AddConstructor<UIComponent, false, false>({});

			type.AddConstructor<UIComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<UIComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::UIComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<UIComponent>>::Register(id, type, false, "411460B5-0A9E-4F82-99E1-73120C63DDAE");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::UIComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<UIComponent>>::Register(id, type, false, "72FF3853-8826-4BFD-B76C-75C37141DC01");
		}

		return true;
	}

	static bool registerGuGuUIComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(UIComponent).GetID()];

		type.AddField<UIComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<UIComponent, std::weak_ptr<GameObject>&, true>::Signature) & UIComponent::getParentGameObject,
			(meta::FieldSetter<UIComponent, std::weak_ptr<GameObject>&, true>::Signature) & UIComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(UIComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuUIComponent)
		IMPLEMENT_INITIAL_END

		IMPLEMENT_INITIAL_FIELDS_BEGIN(UIComponent)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuUIComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	UIComponent::UIComponent()
	{

	}

	UIComponent::~UIComponent()
	{

	}

	meta::Object* UIComponent::Clone(void) const
	{
		return nullptr;
	}

	void UIComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type UIComponent::GetType() const
	{
		return typeof(UIComponent);
	}

	std::shared_ptr<UIDrawInfo> UIComponent::generateUIDrawInformation(bool bFlip)
	{
		return nullptr;
	}

	std::shared_ptr<GuGu::UIDebugInfo> UIComponent::generateUIDebugInfomartion(float inLineWidth)
	{
		std::shared_ptr<UIDebugInfo> debugInfo = std::make_shared<UIDebugInfo>();

		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner)
		{
			float lineWidth = inLineWidth;
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

			uint32_t startIndex = 0;

			//vertex generate
			//first square
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x - lineWidth, absolutePos.y - lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y - lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x - lineWidth, absolutePos.y + localSize.y + lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y + localSize.y + lineWidth, 0));

			//index generate
			debugInfo->m_uiDebugIndices.push_back(0);
			debugInfo->m_uiDebugIndices.push_back(2);
			debugInfo->m_uiDebugIndices.push_back(1);

			debugInfo->m_uiDebugIndices.push_back(1);
			debugInfo->m_uiDebugIndices.push_back(2);
			debugInfo->m_uiDebugIndices.push_back(3);

			//two square
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y - lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y - lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y, 0));
			//
			startIndex = 4;
			//
			////index generate
			debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 3);
			//
			startIndex = 8;
			//
			////three square
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y - lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x + lineWidth, absolutePos.y - lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y + lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x + lineWidth, absolutePos.y + localSize.y + lineWidth, 0));

			debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 3);
			//
			startIndex = 12;

			//four square
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y + localSize.y, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y + localSize.y + lineWidth, 0));
			debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y + lineWidth, 0));

			debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 3);

			//draw anchor

		}

		return debugInfo;
	}

	math::float2 UIComponent::getDesiredSize() const
	{
		return math::float2(0, 0);
	}

	void UIComponent::onPointerDown(UIPointerData pointerData)
	{
		//nothing
	}

	void UIComponent::onPointerUp(UIPointerData pointerData)
	{
		//nothing
	}

}