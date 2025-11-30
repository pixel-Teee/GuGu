#include <pch.h>

#include "GrassComponent.h"
#include <Core/Timer.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {
	static bool registerGuGuGrassComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GrassComponent");
		auto& type = db.types[id];
		meta::TypeInfo<GrassComponent>::Register(id, type, true, "B6816D59-C6ED-48E9-B804-EBA13871A642");

		auto typeID = typeidof(GrassComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GrassComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GrassComponent>();

			type.AddConstructor<GrassComponent, false, false>({});

			type.AddConstructor<GrassComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<GrassComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GrassComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GrassComponent>>::Register(id, type, false, "9753C691-C4D9-484B-B5ED-87E409E0C6F6");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GrassComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GrassComponent>>::Register(id, type, false, "49ACDCAE-7253-4E4A-83CF-B67C2DEF54DE");
		}
		return true;
	}

	static bool registerGuGuGrassComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(GrassComponent).GetID()];

		type.AddField<GrassComponent, Color>("m_bottomColor",
			(meta::FieldGetter<GrassComponent, Color, false>::Signature) & GrassComponent::m_bottomColor,
			(meta::FieldSetter<GrassComponent, Color, false>::Signature) & GrassComponent::m_bottomColor, {});

		type.AddField<GrassComponent, Color>("m_middleColor",
			(meta::FieldGetter<GrassComponent, Color, false>::Signature) & GrassComponent::m_middleColor,
			(meta::FieldSetter<GrassComponent, Color, false>::Signature) & GrassComponent::m_middleColor, {});

		type.AddField<GrassComponent, Color>("m_topColor",
			(meta::FieldGetter<GrassComponent, Color, false>::Signature) & GrassComponent::m_topColor,
			(meta::FieldSetter<GrassComponent, Color, false>::Signature) & GrassComponent::m_topColor, {});

		type.AddField<GrassComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<GrassComponent, std::weak_ptr<GameObject>&, true>::Signature) & GrassComponent::getParentGameObject,
			(meta::FieldSetter<GrassComponent, std::weak_ptr<GameObject>&, true>::Signature) & GrassComponent::setParentGameObject, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GrassComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGrassComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GrassComponent)
		ADD_PRIORITY_FIELDS(Color)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGrassComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GrassComponent::GrassComponent()
	{
		m_bottomColor = Color(0.024f, 0.294f, 0.012f, 1.0f);
		m_middleColor = Color(0.082f, 0.385f, 0.113f, 1.0f);
		m_topColor = Color(0.310f, 1.0f, 0.415f, 1.0f);
	}
	GrassComponent::~GrassComponent()
	{
	}
	meta::Object* GrassComponent::Clone(void) const
	{
		GrassComponent* grassComponent = new GrassComponent();
		grassComponent->m_bottomColor = m_bottomColor;
		grassComponent->m_middleColor = m_middleColor;
		grassComponent->m_topColor = m_topColor;
		return grassComponent;
	}
	void GrassComponent::Update(float fElapsedTimeSeconds)
	{
		//m_lightColor.x += std::cos(fElapsedTimeSeconds * 200.0);
	}
	meta::Type GrassComponent::GetType() const
	{
		return meta::Type(typeof(GrassComponent));
	}

	void GrassComponent::PostLoad()
	{

	}

}