#include <pch.h>

#include "CanvasComponent.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuCanvasomponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::CanvasComponent");
		auto& type = db.types[id];
		meta::TypeInfo<CanvasComponent>::Register(id, type, true, "CF1129A6-E01A-4D4E-8EF5-DB15C100A893");

		auto typeID = typeidof(CanvasComponent);

		return true;
	}

	static bool registerGuGuCanvasComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(CanvasComponent).GetID()];

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(CanvasComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCanvasomponent)
		IMPLEMENT_INITIAL_END

		IMPLEMENT_INITIAL_FIELDS_BEGIN(CanvasComponent)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCanvasComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	CanvasComponent::CanvasComponent()
	{

	}

	CanvasComponent::~CanvasComponent()
	{

	}

	meta::Object* CanvasComponent::Clone(void) const
	{
		return nullptr;
	}

	void CanvasComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type CanvasComponent::GetType() const
	{
		return typeof(CanvasComponent);
	}

}