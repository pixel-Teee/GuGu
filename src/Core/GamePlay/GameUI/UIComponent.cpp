#include <pch.h>

#include "UIComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>
#include <Core/GamePlay/InputManager.h>
#include <Core/GamePlay/UIDragDropOperation.h>

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

		type.AddField<UIComponent, std::shared_ptr<GuGuScriptDelegate>>("m_onPointerDown",
			(meta::FieldGetter<UIComponent, std::shared_ptr<GuGuScriptDelegate>, true>::Signature) & UIComponent::getPointerDownScriptDelegate,
			(meta::FieldSetter<UIComponent, std::shared_ptr<GuGuScriptDelegate>, true>::Signature) & UIComponent::setPointerDownScriptDelegate, {});

		type.AddField<UIComponent, std::shared_ptr<GuGuScriptDelegate>>("m_onPointerUp",
			(meta::FieldGetter<UIComponent, std::shared_ptr<GuGuScriptDelegate>, true>::Signature) & UIComponent::getPointerUpScriptDelegate,
			(meta::FieldSetter<UIComponent, std::shared_ptr<GuGuScriptDelegate>, true>::Signature) & UIComponent::setPointerUpScriptDelegate, {});

		type.AddField<UIComponent, std::shared_ptr<GuGuScriptDelegate>>("m_onPointerMove",
			(meta::FieldGetter<UIComponent, std::shared_ptr<GuGuScriptDelegate>, true>::Signature) & UIComponent::getPointerMoveScriptDelegate,
			(meta::FieldSetter<UIComponent, std::shared_ptr<GuGuScriptDelegate>, true>::Signature) & UIComponent::setPointerMoveScriptDelegate, {});

		type.AddMethod("detectDrag", &UIComponent::detectDrag, {});
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
		m_onPointerDown = std::make_shared<GuGuScriptDelegate>();
		m_onPointerUp = std::make_shared<GuGuScriptDelegate>();
		m_onPointerMove = std::make_shared<GuGuScriptDelegate>();
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

			startIndex = 4;

			//index generate
			debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

			debugInfo->m_uiDebugIndices.push_back(startIndex + 1);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			debugInfo->m_uiDebugIndices.push_back(startIndex + 3);

			startIndex = 8;
			
			//three square
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

			//generate anchor
			//debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y + localSize.y, 0));
			//debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x + localSize.x, absolutePos.y + localSize.y, 0));
			//debugInfo->m_uiDebugVertex.push_back(math::float3(absolutePos.x, absolutePos.y + localSize.y + lineWidth, 0));
			//
			//startIndex = 15;
			//debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
			//debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
			//debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

			//generate anchor
			std::shared_ptr<GameObject> parentGameObject = owner->getParentGameObject().lock();
			if (parentGameObject)
			{
				std::shared_ptr<UITransformComponent> parentUITransformComponent = parentGameObject->getComponent<UITransformComponent>();
				if (parentUITransformComponent)
				{
					//anchor
					UIAnchors anchorDatas = uiTransformComponent->getAnchorData().m_anchors;

					//get global transform
					math::affine3 worldTransform = parentUITransformComponent->GetLocalToWorldTransformFloat();
					math::float3 absolutePos;
					math::quat absoluteQuat;
					math::float3 absoluteScale;
					math::decomposeAffine(worldTransform, &absolutePos, &absoluteQuat, &absoluteScale);
					math::float2 parentLocalSize = parentUITransformComponent->getLocalSize();
					//four corner
					math::float3 firstConer = math::float3(absolutePos.x + anchorDatas.m_minimum.x * parentLocalSize.x, absolutePos.y + (1.0f - anchorDatas.m_minimum.y) * parentLocalSize.y, 0); //(min.x, min.y)
					math::float3 secondConer = math::float3(absolutePos.x + anchorDatas.m_maximum.x * parentLocalSize.x, absolutePos.y + (1.0f - anchorDatas.m_minimum.y) * parentLocalSize.y, 0); //(max.x, min.y)
					math::float3 thirdConer = math::float3(absolutePos.x + anchorDatas.m_minimum.x * parentLocalSize.x, absolutePos.y + (1.0f - anchorDatas.m_maximum.y) * parentLocalSize.y, 0); //(min.x, max.y)
					math::float3 fourthConer = math::float3(absolutePos.x + anchorDatas.m_maximum.x * parentLocalSize.x, absolutePos.y + (1.0f - anchorDatas.m_maximum.y) * parentLocalSize.y, 0); //(max.x, max.y)

					//tan 15
					float tan15 = std::tanf(15.0f / 180.0f * math::PI_f);
					float tan75 = std::tanf(75.0f / 180.0f * math::PI_f);

					//vector
					math::float3 dir = math::normalize(math::float3(-1.0f, tan15, 0));
					math::float3 dir2 = math::normalize(math::float3(-1.0f, tan75, 0));

					debugInfo->m_uiDebugVertex.push_back(firstConer);
					debugInfo->m_uiDebugVertex.push_back(firstConer + dir2 * lineWidth * 5.0f);
					debugInfo->m_uiDebugVertex.push_back(firstConer + dir * lineWidth * 5.0f);

					startIndex = 16;

					debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

					//vector
					math::float3 dir3 = math::normalize(math::float3(1.0f, tan15, 0));
					math::float3 dir4 = math::normalize(math::float3(1.0f, tan75, 0));

					debugInfo->m_uiDebugVertex.push_back(secondConer);
					debugInfo->m_uiDebugVertex.push_back(secondConer + dir3 * lineWidth * 5.0f);
					debugInfo->m_uiDebugVertex.push_back(secondConer + dir4 * lineWidth * 5.0f);

					startIndex = 19;

					debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

					//vector
					math::float3 dir5 = math::normalize(math::float3(-1.0f, -tan15, 0));
					math::float3 dir6 = math::normalize(math::float3(-1.0f, -tan75, 0));

					debugInfo->m_uiDebugVertex.push_back(thirdConer);
					debugInfo->m_uiDebugVertex.push_back(thirdConer + dir5 * lineWidth * 5.0f);
					debugInfo->m_uiDebugVertex.push_back(thirdConer + dir6 * lineWidth * 5.0f);

					startIndex = 22;

					debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 1);

					math::float3 dir7 = math::normalize(math::float3(1.0f, -tan15, 0));
					math::float3 dir8 = math::normalize(math::float3(1.0f, -tan75, 0));

					debugInfo->m_uiDebugVertex.push_back(fourthConer);
					debugInfo->m_uiDebugVertex.push_back(fourthConer + dir8 * lineWidth * 5.0f);
					debugInfo->m_uiDebugVertex.push_back(fourthConer + dir7 * lineWidth * 5.0f);

					startIndex = 25;

					debugInfo->m_uiDebugIndices.push_back(startIndex + 0);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 2);
					debugInfo->m_uiDebugIndices.push_back(startIndex + 1);
				}
			}
		}

		return debugInfo;
	}

	math::float2 UIComponent::getDesiredSize() const
	{
		return math::float2(0, 0);
	}

	void UIComponent::onPointerDown(UIPointerData pointerData)
	{
		if(m_onPointerDown)
		{
			std::vector<meta::Variant> args;
			args.push_back(pointerData);
			m_onPointerDown->invoke(args);
		}
	}

	void UIComponent::onPointerUp(UIPointerData pointerData)
	{
		if(m_onPointerUp)
		{
			std::vector<meta::Variant> args;
			args.push_back(pointerData);
			m_onPointerUp->invoke(args);
		}
	}

    void UIComponent::onPointerMove(UIPointerData pointerData)
    {
		if(m_bDragDetectionEnabled)
		{
			math::float2 delta = pointerData.getScreenPosition() - m_dragDetectionStartPos;
			if(math::length(delta) >= m_dragDetectionThreshold)
			{
				//notify global to start drag
				m_bIsDragging = true;
				std::shared_ptr<UIDragDropOperation> operation = std::make_shared<UIDragDropOperation>();
				InputManager::getInputManager()->startDrag(std::static_pointer_cast<UIComponent>(shared_from_this()), operation);
			}
			m_bDragDetectionEnabled = false;
			return;
		}

		if(m_onPointerMove)
		{
			std::vector<meta::Variant> args;
			args.push_back(pointerData);
			m_onPointerMove->invoke(args);
		}
    }

    std::shared_ptr<GuGuScriptDelegate> UIComponent::getPointerDownScriptDelegate()
    {
        return m_onPointerDown;
    }

    void UIComponent::setPointerDownScriptDelegate(std::shared_ptr<GuGuScriptDelegate> inScriptDelegate)
    {
		m_onPointerDown = inScriptDelegate;
    }

    std::shared_ptr<GuGuScriptDelegate> UIComponent::getPointerUpScriptDelegate()
    {
        return m_onPointerUp;
    }

    void UIComponent::setPointerUpScriptDelegate(std::shared_ptr<GuGuScriptDelegate> inScriptDelegate)
    {
		m_onPointerUp = inScriptDelegate;
    }

    std::shared_ptr<GuGuScriptDelegate> UIComponent::getPointerMoveScriptDelegate()
    {
        return m_onPointerMove;
    }

    void UIComponent::setPointerMoveScriptDelegate(std::shared_ptr<GuGuScriptDelegate> inScriptDelegate)
    {
		m_onPointerMove = inScriptDelegate;
    }

    void UIComponent::detectDrag(float threshold, math::float2 inStartPos)
    {
		m_bDragDetectionEnabled = true;
		m_dragDetectionStartPos = inStartPos;
		m_dragDetectionThreshold = threshold;
    }
}