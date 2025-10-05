#include <pch.h>

#include "GAnimation.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerGuGuGAnimation()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GAnimation");
			auto& type = db.types[id];
			meta::TypeInfo<GAnimation>::Register(id, type, true, "21C77D30-0352-49D1-A547-E71404B53BC7");

			auto typeID = typeidof(GAnimation);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GAnimation>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GAnimation>();

				type.AddConstructor<GAnimation, false, false>({});

				type.AddConstructor<GAnimation, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<GAnimation>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::GAnimation>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GAnimation>>::Register(id, type, false, "38512377-656E-4B5E-B217-F763DBFAD196");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::GAnimation>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GAnimation>>::Register(id, type, false, "EF61A1B3-E9DE-4BFF-8A22-BD44B3DD1ECA");
			}
		}
		return true;
	}
	static bool registerGuGuGAnimationFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(GAnimation).GetID()];

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GAnimation)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGAnimation)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GAnimation)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGAnimationFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GAnimation::GAnimation()
	{
	}
	GAnimation::~GAnimation()
	{
	}

	void GAnimation::PostLoad()
	{
		
	}

	void GAnimation::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type GAnimation::GetType() const
	{
		return typeof(GAnimation);
	}

	meta::Object* GAnimation::Clone(void) const
	{
		GAnimation* staticMesh = new GAnimation();

		return staticMesh;
	}

	void GAnimation::OnSerialize(nlohmann::json& output) const
	{

	}

	void GAnimation::OnDeserialize(const nlohmann::json& input)
	{
		
	}

}