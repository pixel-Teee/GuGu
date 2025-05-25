#include <pch.h>

#include "LightComponent.h"
#include <Core/Timer.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuLightComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::LightComponent");
		auto& type = db.types[id];
		meta::TypeInfo<LightComponent>::Register(id, type, true, "3EF471CA-C9A2-4F2B-AEA9-326276AE4763");

		auto typeID = typeidof(LightComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<LightComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<LightComponent>();

			type.AddConstructor<LightComponent, false, false>({});

			type.AddConstructor<LightComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<LightComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::LightComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<LightComponent>>::Register(id, type, false, "1E0975BC-3D79-466A-A383-C7EAAEB21605");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::LightComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<LightComponent>>::Register(id, type, false, "77075968-2FB0-40BC-A431-C57C530A6724");
		}
		return true;
	}

	static bool registerGuGuLightComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(LightComponent).GetID()];
		
		type.AddField<LightComponent, Color>("m_lightColor",
			(meta::FieldGetter<LightComponent, Color, false>::Signature) & LightComponent::m_lightColor,
			(meta::FieldSetter<LightComponent, Color, false>::Signature) & LightComponent::m_lightColor, {});

		type.AddField<LightComponent, math::float4>("m_lightPosition",
			(meta::FieldGetter<LightComponent, math::float4, false>::Signature) & LightComponent::m_lightPosition,
			(meta::FieldSetter<LightComponent, math::float4, false>::Signature) & LightComponent::m_lightPosition, {});

		type.AddField<LightComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature) & LightComponent::getParentGameObject,
			(meta::FieldSetter<LightComponent, std::weak_ptr<GameObject>&, true>::Signature) & LightComponent::setParentGameObject, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(LightComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuLightComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(LightComponent)
		ADD_PRIORITY_FIELDS(Color)
		ADD_PRIORITY_FIELDS(GameObject)
		if (!ms_priority2.addPriorityThan(&mathfloat4Priority)) return 0; //add priority
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuLightComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END
	LightComponent::LightComponent()
	{
		m_lightPosition = math::float4(0.0f, 0.0f, -10.0f, 1.0f);
		m_lightColor = Color(200.0f, 200.0f, 900.0f, 1.0f);
	}
	LightComponent::~LightComponent()
	{
	}
	meta::Object* LightComponent::Clone(void) const
	{
		LightComponent* lightComponent = new LightComponent();
		lightComponent->m_lightPosition = m_lightPosition;
		lightComponent->m_lightColor = m_lightColor;
		return lightComponent;
	}
	void LightComponent::Update(float fElapsedTimeSeconds)
	{
		//m_lightColor.x += std::cos(fElapsedTimeSeconds * 200.0);
	}
	meta::Type LightComponent::GetType() const
	{
		return meta::Type(typeof(LightComponent));
	}
}