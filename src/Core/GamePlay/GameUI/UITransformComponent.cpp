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

		return true;
	}

	static bool registerGuGuUITransformComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(UITransformComponent).GetID()];

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

	}

	meta::Type UITransformComponent::GetType() const
	{
		return typeof(UITransformComponent);
	}

}