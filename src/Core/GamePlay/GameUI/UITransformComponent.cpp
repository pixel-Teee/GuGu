#include <pch.h>

#include "UITransformComponent.h"
#include "UIComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

#include <core/Reflection/MetaProperty/HiddenProperty.h> //hidden property

namespace GuGu {
	static bool registerGuGuUITransformComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UITransformComponent");
		auto& type = db.types[id];
		meta::TypeInfo<UITransformComponent>::Register(id, type, true, "4253446C-BF55-4825-AD2E-E698765C902B");

		auto typeID = typeidof(UITransformComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<UITransformComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<UITransformComponent>();

			type.AddConstructor<UITransformComponent, false, false>({});

			type.AddConstructor<UITransformComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(TransformComponent) });

			meta::TypeInfo<UITransformComponent>::Defined = true;

// 			type.meta = {
// 				std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("TransformComponent"))
// 			};
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::UITransformComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<UITransformComponent>>::Register(id, type, false, "9124CC89-0D58-4831-9E41-F4478D0A59FF");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::UITransformComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<UITransformComponent>>::Register(id, type, false, "96B36525-5F32-4602-893F-434A9A9E9EB0");
		}
		return true;
	}

	static bool registerGuGuUITransformComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(UITransformComponent).GetID()];
		type.AddField<UITransformComponent, math::double3>("m_Translation",
			(meta::FieldGetter<UITransformComponent, math::double3&, true>::Signature) & UITransformComponent::getTranslation,
			(meta::FieldSetter<UITransformComponent, math::double3, true>::Signature) & UITransformComponent::SetTranslation, {});
		//type.AddField<TransformComponent, math::dquat>("m_Rotation",
		//	(meta::FieldGetter<TransformComponent, math::dquat, true>::Signature) & TransformComponent::getRotation,
		//	(meta::FieldSetter<TransformComponent, math::dquat, true>::Signature) & TransformComponent::SetRotation, {});
		type.AddField<UITransformComponent, math::Rotator>("m_Rotation",
			(meta::FieldGetter<UITransformComponent, math::Rotator&, true>::Signature) & UITransformComponent::getRotator,
			(meta::FieldSetter<UITransformComponent, math::Rotator, true>::Signature) & UITransformComponent::SetRotator, {});
		type.AddField<UITransformComponent, math::double3>("m_Scaling",
			(meta::FieldGetter<UITransformComponent, math::double3&, true>::Signature) & UITransformComponent::getScaling,
			(meta::FieldSetter<UITransformComponent, math::double3, true>::Signature) & UITransformComponent::SetScaling, {});

		type.AddField<UITransformComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<UITransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & UITransformComponent::getParentGameObject,
			(meta::FieldSetter<UITransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & UITransformComponent::setParentGameObject, 
			{
				std::make_pair(typeof(meta::HiddenProperty), meta::MetaPropertyInitializer<meta::HiddenProperty>())
			}
		);

		//type.AddField<UITransformComponent, UIPadding>("m_offset",
		//	(meta::FieldGetter<UITransformComponent, UIPadding&, true>::Signature) & UITransformComponent::getUIOffset,
		//	(meta::FieldSetter<UITransformComponent, UIPadding, true>::Signature) & UITransformComponent::setUIOffset, {});
		//
		//type.AddField<UITransformComponent, UIAnchors>("m_anchors",
		//	(meta::FieldGetter<UITransformComponent, UIAnchors&, true>::Signature) & UITransformComponent::getUIAnchors,
		//	(meta::FieldSetter<UITransformComponent, UIAnchors, true>::Signature) & UITransformComponent::setUIAnchors, {});

		//type.AddField<UITransformComponent, UIAnchors>("m_anchors",
		//	(meta::FieldGetter<UITransformComponent, UIAnchors&, true>::Signature) & UITransformComponent::getUIAnchors,
		//	(meta::FieldSetter<UITransformComponent, UIAnchors, true>::Signature) & UITransformComponent::setUIAnchors, {});

		//type.AddField<UITransformComponent, UIAnchors>("m_anchors",
		//	(meta::FieldGetter<UITransformComponent, UIAnchors&, true>::Signature) & UITransformComponent::getUIAnchors,
		//	(meta::FieldSetter<UITransformComponent, UIAnchors, true>::Signature) & UITransformComponent::setUIAnchors, {});

		//type.AddField<UITransformComponent, math::float2>("m_alignment",
		//	(meta::FieldGetter<UITransformComponent, math::float2&, true>::Signature) & UITransformComponent::getAlignment,
		//	(meta::FieldSetter<UITransformComponent, math::float2, true>::Signature) & UITransformComponent::setAlignment, {});

		type.AddField<UITransformComponent, bool>("m_autoSize",
			(meta::FieldGetter<UITransformComponent, bool&, true>::Signature) & UITransformComponent::getAutoSize,
			(meta::FieldSetter<UITransformComponent, bool, true>::Signature) & UITransformComponent::setAutoSize, {});

		type.AddField<UITransformComponent, UIAnchorData>("m_anchorData",
			(meta::FieldGetter<UITransformComponent, UIAnchorData&, true>::Signature) & UITransformComponent::getAnchorData,
			(meta::FieldSetter<UITransformComponent, UIAnchorData, true>::Signature) & UITransformComponent::setAnchorData, {});

		//type.AddField<UITransformComponent, math::double3>("m_Scaling",
		//	(meta::FieldGetter<UITransformComponent, math::double3&, true>::Signature) & UITransformComponent::getScaling,
		//	(meta::FieldSetter<UITransformComponent, math::double3, true>::Signature) & UITransformComponent::SetScaling, {});
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(UITransformComponent)
		ADD_PRIORITY(TransformComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuUITransformComponent)
		IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(UITransformComponent)
		if (!ms_priority2.addPriorityThan(&uiPaddingPriority)) return 0; //add priority
		if (!ms_priority2.addPriorityThan(&uiAnchorsPriority)) return 0; //add priority
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_PRIORITY_FIELDS(meta::HiddenProperty)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuUITransformComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	UITransformComponent::UITransformComponent()
	{
		m_Translation = math::double3(0.0f, 0.0f, 0.0f);
		m_Scaling = math::double3(1.0f, 1.0f, 1.0f);
		m_Rotation = math::Rotator();

		m_anchorData.m_anchors = UIAnchors(math::float2(0, 0), math::float2(1, 1));
		m_anchorData.m_offset = UIPadding(0, 0, 0, 0);
		m_anchorData.m_alignment = math::float2(0, 0);
		m_zOrder = 0;
		m_localSize = math::float2(0, 0);
		m_localPosition = math::float2(0, 0);
	}

	UITransformComponent::~UITransformComponent()
	{

	}

	meta::Object* UITransformComponent::Clone(void) const
	{
		UITransformComponent* transformComponent = new UITransformComponent();
		transformComponent->m_Translation = m_Translation;
		transformComponent->m_Scaling = m_Scaling;
		transformComponent->m_Rotation = m_Rotation;
		transformComponent->m_owner = m_owner;//todo:这里要修复

		transformComponent->m_anchorData.m_anchors = m_anchorData.m_anchors; //anchor
		transformComponent->m_anchorData.m_offset = m_anchorData.m_offset; //offset
		transformComponent->m_anchorData.m_alignment = m_anchorData.m_alignment;
		return transformComponent;
	}

	void UITransformComponent::Update(float fElapsedTimeSeconds)
	{
		//calculate layout
		calculateLayout();

		TransformComponent::Update(fElapsedTimeSeconds);
	}

	meta::Type UITransformComponent::GetType() const
	{
		return typeof(UITransformComponent);
	}

	UIPadding UITransformComponent::getUIOffset() const
	{
		return m_anchorData.m_offset;
	}

	UIPadding& UITransformComponent::getUIOffset()
	{
		return m_anchorData.m_offset;
	}

	void UITransformComponent::setUIOffset(UIPadding inUIPadding)
	{
		m_anchorData.m_offset = inUIPadding;
	}

	UIAnchors UITransformComponent::getUIAnchors() const
	{
		return m_anchorData.m_anchors;
	}

	UIAnchors& UITransformComponent::getUIAnchors()
	{
		return m_anchorData.m_anchors;
	}

	void UITransformComponent::setUIAnchors(UIAnchors inUIAnchors)
	{
		m_anchorData.m_anchors = inUIAnchors;
	}

	math::float2 UITransformComponent::getAlignment() const
	{
		return m_anchorData.m_alignment;
	}

	math::float2& UITransformComponent::getAlignment()
	{
		return m_anchorData.m_alignment;
	}

	void UITransformComponent::setAlignment(math::float2 inAlignment)
	{
		m_anchorData.m_alignment = inAlignment;
	}

	bool UITransformComponent::getAutoSize() const
	{
		return m_autoSize;
	}

	bool& UITransformComponent::getAutoSize()
	{
		return m_autoSize;
	}

	void UITransformComponent::setAutoSize(bool autoSize)
	{
		m_autoSize = autoSize;
	}

	float UITransformComponent::getZOrder() const
	{
		return m_zOrder;
	}

	float& UITransformComponent::getZOrder()
	{
		return m_zOrder;
	}

	void UITransformComponent::setZOrder(float zOrder)
	{
		m_zOrder = zOrder;
	}

	UIAnchorData UITransformComponent::getAnchorData() const
	{
		return m_anchorData;
	}

	UIAnchorData& UITransformComponent::getAnchorData()
	{
		return m_anchorData;
	}

	void UITransformComponent::setAnchorData(UIAnchorData inUIAnchorData)
	{
		m_anchorData = inUIAnchorData;
	}

	math::float2 UITransformComponent::getLocalSize() const
	{
		return m_localSize;
	}

	math::float2& UITransformComponent::getLocalSize()
	{
		return m_localSize;
	}

	void UITransformComponent::setLocalSize(math::float2 inLocalSize)
	{
		m_localSize = inLocalSize;
	}

	math::float2 UITransformComponent::getLocalPosition() const
	{
		return m_localPosition;
	}

	math::float2& UITransformComponent::getLocalPosition()
	{
		return m_localPosition;
	}

	void UITransformComponent::setLocalPosition(math::float2 inLocalPosition)
	{
		m_localPosition = inLocalPosition;
	}

	void UITransformComponent::setScaleFactor(float inScaleFactor)
	{
		m_scaleFactor = inScaleFactor;
	}

	float UITransformComponent::getScaleFactor() const
	{
		return m_scaleFactor;
	}

	void UITransformComponent::calculateLayout()
	{
		//get children
		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner != nullptr)
		{
			//float inverseScale = 1.0f / m_scaleFactor;
			Array<std::shared_ptr<GameObject>>& childrens = owner->getChildrens();
			for (int32_t i = 0; i < childrens.size(); ++i)
			{
				std::shared_ptr<GameObject> object = childrens[i];
				if (object != nullptr)
				{
					std::shared_ptr<UITransformComponent> uiTransformComponent = object->getComponent<UITransformComponent>();
					if (uiTransformComponent)
					{
						const UIPadding offset = uiTransformComponent->getUIOffset();
						const math::float2 alignment = uiTransformComponent->getAlignment();
						const UIAnchors anchors = uiTransformComponent->getUIAnchors();
						const bool autoSize = uiTransformComponent->getAutoSize();

						const UIPadding anchorPixels = UIPadding(
							anchors.m_minimum.x * m_localSize.x,//left
							anchors.m_minimum.y * m_localSize.y,//top
							anchors.m_maximum.x * m_localSize.x,//right
							anchors.m_maximum.y * m_localSize.y//bottom
						);

						const bool bIsHorizontalStretch = anchors.m_minimum.x != anchors.m_maximum.x;
						const bool bIsVerticalStretch = anchors.m_minimum.y != anchors.m_maximum.y;

						const math::float2 slotSize = math::float2(offset.right, offset.bottom);

						//todo:handle auto size
						const math::float2 widgetFixedSize = slotSize;

						math::float2 alignmentOffset = widgetFixedSize * alignment;//pivot

						math::float2 localPosition, localSize;
						if (bIsHorizontalStretch)
						{
							localPosition.x = anchorPixels.left + offset.left;
							localSize.x = anchorPixels.right - localPosition.x - offset.right;
						}
						else
						{
							localPosition.x = anchorPixels.left + offset.left - alignmentOffset.x;
							localSize.x = widgetFixedSize.x;
						}
						if (bIsVerticalStretch)
						{
							localPosition.y = m_localSize.y - (anchorPixels.top + offset.top);
							//localSize.y = anchorPixels.bottom - localPosition.y - offset.bottom;
							localSize.y = -(anchorPixels.bottom - offset.bottom);
						}
						else
						{
							//localPosition.y = anchorPixels.top + offset.top - alignmentOffset.y;
							localPosition.y = m_localSize.y - (anchorPixels.top + offset.top + alignmentOffset.y);
							localSize.y = widgetFixedSize.y;
						}

						//uiTransformComponent->setScaleFactor(m_scaleFactor);
						uiTransformComponent->setLocalPosition(localPosition);
						uiTransformComponent->setLocalSize(localSize);
						uiTransformComponent->SetTranslation(math::double3(localPosition.x, localPosition.y, m_zOrder - 0.1));//local trans
						uiTransformComponent->setZOrder(m_zOrder - 0.1);
					}
				}
			}
		}
	}

}