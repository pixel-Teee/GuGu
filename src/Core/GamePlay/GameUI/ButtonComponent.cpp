#include <pch.h>

#include "ButtonComponent.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuButtonComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::ButtonComponent");
		auto& type = db.types[id];
		meta::TypeInfo<ButtonComponent>::Register(id, type, true, "E42F9928-A982-45DC-B83B-06875D3051DE");

		auto typeID = typeidof(ButtonComponent);

		return true;
	}

	static bool registerGuGuButtonComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(ButtonComponent).GetID()];

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(ButtonComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuButtonComponent)
		IMPLEMENT_INITIAL_END

		IMPLEMENT_INITIAL_FIELDS_BEGIN(ButtonComponent)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuButtonComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	ButtonComponent::ButtonComponent()
	{

	}

	ButtonComponent::~ButtonComponent()
	{

	}

	meta::Object* ButtonComponent::Clone(void) const
	{
		return nullptr;
	}

	void ButtonComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type ButtonComponent::GetType() const
	{
		return typeof(ButtonComponent);
	}

}