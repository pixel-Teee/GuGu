#include <pch.h>

#include "UITransformComponent.h"
#include <Core/Reflection/TypeInfo.h>

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

			type.LoadBaseClasses(db, typeID, { typeof(UITransformComponent) });

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
			(meta::FieldSetter<UITransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & UITransformComponent::setParentGameObject, {});

		type.AddField<UITransformComponent, UIPadding>("m_offset",
			(meta::FieldGetter<UITransformComponent, UIPadding&, true>::Signature) & UITransformComponent::getUIPadding,
			(meta::FieldSetter<UITransformComponent, UIPadding, true>::Signature) & UITransformComponent::setUIPadding, {});

		type.AddField<UITransformComponent, UIAnchors>("m_anchors",
			(meta::FieldGetter<UITransformComponent, UIAnchors&, true>::Signature) & UITransformComponent::getUIAnchors,
			(meta::FieldSetter<UITransformComponent, UIAnchors, true>::Signature) & UITransformComponent::setUIAnchors, {});

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
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuUITransformComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	UITransformComponent::UITransformComponent()
	{

	}

	UITransformComponent::~UITransformComponent()
	{

	}

	meta::Object* UITransformComponent::Clone(void) const
	{
		return nullptr;
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

	

	void UITransformComponent::calculateLayout()
	{

	}

}