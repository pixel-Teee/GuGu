#include <pch.h>

#include "UIComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

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

}