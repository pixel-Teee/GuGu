#include <pch.h>

#include "Collision3DComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuCollision3DComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Collision3DComponent");
		auto& type = db.types[id];
		meta::TypeInfo<Collision3DComponent>::Register(id, type, true, "697F186C-FB1C-43E8-9FE2-8A468A7936BE");

		auto typeID = typeidof(Collision3DComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Collision3DComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<Collision3DComponent>();

			type.AddConstructor<Collision3DComponent, false, false>({});

			type.AddConstructor<Collision3DComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<Collision3DComponent>::Defined = true;

			// 			type.AddField<Collision3DComponent, float>("m_fov",
			// 				(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getFov,
			// 				(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setFov, {});
			// 
			// 			type.AddField<Collision3DComponent, float>("m_nearPlane",
			// 				(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getNearPlane,
			// 				(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setNearPlane, {});
			// 
			// 			type.AddField<Collision3DComponent, float>("m_farPlane",
			// 				(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getFarPlane,
			// 				(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setFarPlane, {});
			// 
			// 			type.AddField<Collision3DComponent, std::weak_ptr<GameObject>>("m_owner",
			// 				(meta::FieldGetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::getParentGameObject,
			// 				(meta::FieldSetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<Collision3DComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Collision3DComponent>>::Register(id, type, false, "D866D79C-DC28-4F6B-AF00-E6E592E3C8A3");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<Collision3DComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Collision3DComponent>>::Register(id, type, false, "1E51EEDA-262E-4608-A995-864D1CB5B6C3");
		}
		return true;
	}

	static bool registerGuGuCollision3DComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(Collision3DComponent).GetID()];

		type.AddField<Collision3DComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::getParentGameObject,
			(meta::FieldSetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(Collision3DComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCollision3DComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(Collision3DComponent)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCollision3DComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	Collision3DComponent::Collision3DComponent()
	{
		
	}

	Collision3DComponent::~Collision3DComponent()
	{

	}

	meta::Object* Collision3DComponent::Clone(void) const
	{
		Collision3DComponent* collision3d = new Collision3DComponent();

		return collision3d;
	}

	void Collision3DComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type Collision3DComponent::GetType() const
	{
		return typeof(Collision3DComponent);
	}

}

