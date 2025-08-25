#include <pch.h>

#include "TransformComponent.h"
#include "GameObject.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/Reflection/MetaProperty/DisplayName.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerTransformComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		//各种 component
		{
			auto id = db.AllocateType("GuGu::TransformComponent");
			auto& type = db.types[id];
			meta::TypeInfo<TransformComponent>::Register(id, type, true, "57B43954-87AF-495A-8B58-E15890E621D1");

			auto typeID = typeidof(TransformComponent);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<TransformComponent>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<TransformComponent>();

				type.AddConstructor<TransformComponent, false, false>({});

				type.AddConstructor<TransformComponent, true, true>({});

				type.LoadBaseClasses(db, typeID, { typeof(Component) });

				meta::TypeInfo<TransformComponent>::Defined = true;

				type.meta = {
					std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("TransformComponent"))
				};
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::TransformComponent>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<TransformComponent>>::Register(id, type, false, "311DBA9A-2885-4656-882D-81FF8EF66502");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::TransformComponent>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<TransformComponent>>::Register(id, type, false, "DC63130F-4532-4F84-9A37-11567967CEFA");
			}
		}
		return true;
	}

	static bool registerTransformComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(TransformComponent).GetID()];
		type.AddField<TransformComponent, math::double3>("m_Translation",
			(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature) & TransformComponent::getTranslation,
			(meta::FieldSetter<TransformComponent, math::double3, true>::Signature) & TransformComponent::SetTranslation, {});
		//type.AddField<TransformComponent, math::dquat>("m_Rotation",
		//	(meta::FieldGetter<TransformComponent, math::dquat, true>::Signature) & TransformComponent::getRotation,
		//	(meta::FieldSetter<TransformComponent, math::dquat, true>::Signature) & TransformComponent::SetRotation, {});
		type.AddField<TransformComponent, math::Rotator>("m_Rotation",
			(meta::FieldGetter<TransformComponent, math::Rotator&, true>::Signature) & TransformComponent::getRotator,
			(meta::FieldSetter<TransformComponent, math::Rotator, true>::Signature) & TransformComponent::SetRotator, {});
		type.AddField<TransformComponent, math::double3>("m_Scaling",
			(meta::FieldGetter<TransformComponent, math::double3&, true>::Signature) & TransformComponent::getScaling,
			(meta::FieldSetter<TransformComponent, math::double3, true>::Signature) & TransformComponent::SetScaling, {});

		type.AddField<TransformComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & TransformComponent::getParentGameObject,
			(meta::FieldSetter<TransformComponent, std::weak_ptr<GameObject>&, true>::Signature) & TransformComponent::setParentGameObject, {});

		//------add method------
		type.AddMethod("SetTranslation", &TransformComponent::SetTranslation, {});

		math::double3&(TransformComponent::*getTranslation)() = &TransformComponent::getTranslation;
		type.AddMethod("getTranslation", getTranslation, {});
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(TransformComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerTransformComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(TransformComponent)
		ADD_PRIORITY_FIELDS(meta::DisplayName)
		if (!ms_priority2.addPriorityThan(&mathdouble3Priority)) return 0; //add priority
		if (!ms_priority2.addPriorityThan(&mathdquatPriority)) return 0; //add priority
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerTransformComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END
	TransformComponent::TransformComponent()
	{
	}
	TransformComponent::~TransformComponent()
	{
	}

	meta::Object* TransformComponent::Clone(void) const
	{
		TransformComponent* transformComponent = new TransformComponent();
		transformComponent->m_Translation = m_Translation;
		transformComponent->m_Scaling = m_Scaling;
		transformComponent->m_Rotation = m_Rotation;
		transformComponent->m_owner = m_owner;//todo:这里要修复
		return transformComponent;
	}

	void TransformComponent::Update(float fElapsedTimeSeconds)
	{	
		UpdateLocalModelMatrix();
		UpdateGlobalTransform();
	}
	void TransformComponent::UpdateLocalModelMatrix()
	{
		//update local transform
		math::daffine3 transform = math::scaling(m_Scaling);
		transform *= math::rotationQuat(m_Rotation.getRadians()).toAffine();
		transform *= math::translation(m_Translation);
		m_LocalTransform = transform;
	}
	void TransformComponent::UpdateGlobalTransform()
	{
		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner != nullptr)
		{	
			std::shared_ptr<GameObject> parentGameObject = owner->getParentGameObject().lock();

			if (parentGameObject != nullptr)
			{
				std::shared_ptr<TransformComponent> parentTransformComponent =
					parentGameObject->getComponent<TransformComponent>();
				m_GlobalTransform = parentTransformComponent->GetLocalToWorldTransform() * m_LocalTransform;
			}
			else
			{
				m_GlobalTransform = m_LocalTransform;
			}
		}
		else
		{
			m_GlobalTransform = m_LocalTransform;
		}
		m_GlobalTransformFloat = math::affine3(m_GlobalTransform);
	}
	void TransformComponent::SetTransform(const math::double3* translation, const math::dquat* rotation, const math::double3* scaling)
	{
		if (scaling) m_Scaling = *scaling;
		if (rotation) m_Rotation = math::quatToEuler(*rotation);
		if (translation) m_Translation = *translation;
	}
	void TransformComponent::SetScaling(math::double3 scaling)
	{
		SetTransform(nullptr, nullptr, &scaling);
	}
	void TransformComponent::SetRotationQuat(math::dquat rotation)
	{
		SetTransform(nullptr, &rotation, nullptr);
	}

	void TransformComponent::SetRotator(math::Rotator inRotator)
	{
		//SetTransform(nullptr, &rotation, nullptr);
		m_Rotation = inRotator;
	}

	void TransformComponent::SetTranslation(math::double3 translation)
	{
		SetTransform(&translation, nullptr, nullptr);
	}

	math::double3 TransformComponent::getTranslation() const
	{
		return m_Translation;
	}

	math::double3& TransformComponent::getTranslation()
	{
		return m_Translation;
	}

	GuGu::math::dquat TransformComponent::getRotationQuat() const
{
		return math::rotationQuat(m_Rotation.getRadians());
	}

	GuGu::math::Rotator TransformComponent::getRotator() const
	{
		return m_Rotation;
	}

	GuGu::math::Rotator& TransformComponent::getRotator()
	{
		return m_Rotation;
	}

	math::double3 TransformComponent::getScaling() const
	{
		return m_Scaling;
	}

	math::double3& TransformComponent::getScaling()
	{
		return m_Scaling;
	}

// 	math::double3 TransformComponent::getAccumulatedTranslation() const
// 	{
// 		math::double3 accumulatedTranslation = math::double3(0, 0, 0);
// 		std::shared_ptr<GameObject> owner = m_owner.lock();
// 		if (owner != nullptr)
// 		{
// 			std::shared_ptr<GameObject> parentGameObject = owner->getParentGameObject().lock();
// 
// 			if (parentGameObject != nullptr)
// 			{
// 				std::shared_ptr<TransformComponent> parentTransformComponent =
// 					parentGameObject->getComponent<TransformComponent>();
// 				accumulatedTranslation = parentTransformComponent->getAccumulatedTranslation() + accumulatedTranslation;
// 			}
// 		}
// 		return accumulatedTranslation;
// 	}

	meta::Type TransformComponent::GetType() const
	{
		return typeof(TransformComponent);
	}
}