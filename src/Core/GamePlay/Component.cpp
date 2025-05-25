#include <pch.h>

#include "Component.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Component");
		auto& type = db.types[id];
		meta::TypeInfo<Component>::Register(id, type, true, "0955EA47-6CB3-4F4E-929A-A19769B4DD93");

		type.AddField<Component, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<Component, std::weak_ptr<GameObject>&, true>::Signature) & Component::getParentGameObject,
			(meta::FieldSetter<Component, std::weak_ptr<GameObject>&, true>::Signature) & Component::setParentGameObject, {});

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::Component>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Component>>::Register(id, type, false, "D7753E39-309C-4C52-BF1D-BCA0F90D6352");

			type.SetArrayConstructor<std::shared_ptr<Component>>();
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::Component>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Component>>::Register(id, type, false, "1CA55F88-8D64-445F-AE1B-E6635525E0DA");
		}
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuComponent)
	IMPLEMENT_INITIAL_END

    meta::Type Component::GetType() const
    {
        return typeof(Component);
    }

	const std::weak_ptr<GameObject>& Component::getParentGameObject() const
	{
		return m_owner;
	}

	std::weak_ptr<GameObject>& Component::getParentGameObject()
	{
		return m_owner;
	}

	void Component::setParentGameObject(const std::weak_ptr<GameObject>& inGameObject)
	{
		m_owner = inGameObject;
	}
}