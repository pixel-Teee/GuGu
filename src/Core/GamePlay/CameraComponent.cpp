#include <pch.h>

#include "CameraComponent.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuCameraComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::CameraComponent");
		auto& type = db.types[id];
		meta::TypeInfo<CameraComponent>::Register(id, type, true, "DE850877-05B1-487C-A11F-CBABB546D177");

		auto typeID = typeidof(CameraComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<CameraComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<CameraComponent>();

			type.AddConstructor<CameraComponent, false, false>({});

			type.AddConstructor<CameraComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<CameraComponent>::Defined = true;

			type.AddField<CameraComponent, float>("m_fov",
				(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getFov,
				(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setFov, {});

			type.AddField<CameraComponent, float>("m_nearPlane",
				(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getNearPlane,
				(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setNearPlane, {});

			type.AddField<CameraComponent, float>("m_farPlane",
				(meta::FieldGetter<CameraComponent, float, true>::Signature) & CameraComponent::getFarPlane,
				(meta::FieldSetter<CameraComponent, float, true>::Signature) & CameraComponent::setFarPlane, {});

			type.AddField<CameraComponent, std::weak_ptr<GameObject>>("m_owner",
				(meta::FieldGetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature) & CameraComponent::getParentGameObject,
				(meta::FieldSetter<CameraComponent, std::weak_ptr<GameObject>&, true>::Signature) & CameraComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<CameraComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<CameraComponent>>::Register(id, type, false, "D866D79C-DC28-4F6B-AF00-E6E592E3C8A3");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<CameraComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<CameraComponent>>::Register(id, type, false, "1E51EEDA-262E-4608-A995-864D1CB5B6C3");
		}
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(CameraComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCameraComponent)
	IMPLEMENT_INITIAL_END

	CameraComponent::CameraComponent()
	{
		m_fov = 60.0f;
		
	}

	CameraComponent::~CameraComponent()
	{

	}

	meta::Object* CameraComponent::Clone(void) const
	{
		return nullptr;
	}

	void CameraComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type CameraComponent::GetType() const
	{
		return typeof(CameraComponent);
	}

	float CameraComponent::getFov() const
	{
		return m_fov;
	}

	void CameraComponent::setFov(float fov)
	{
		m_fov = fov;
	}

	float CameraComponent::getNearPlane() const
	{
		return m_nearPlane;
	}

	void CameraComponent::setNearPlane(float nearPlane)
	{
		m_nearPlane = nearPlane;
	}

	float CameraComponent::getFarPlane() const
	{
		return m_farPlane;
	}

	void CameraComponent::setFarPlane(float farPlane)
	{
		m_farPlane = farPlane;
	}

}

