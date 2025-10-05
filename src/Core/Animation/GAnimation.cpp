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
			meta::TypeInfo<GAnimation>::Register(id, type, true, "2B9E11D1-1D66-457E-8CE9-60BE82518687");

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

		type.AddField<GAnimation, int32_t>("m_ticksPerSecond",
			(meta::FieldGetter<GAnimation, int32_t, false>::Signature) & GAnimation::m_ticksPerSecond,
			(meta::FieldSetter<GAnimation, int32_t, false>::Signature) & GAnimation::m_ticksPerSecond, {});

		type.AddField<GAnimation, float>("m_duration",
			(meta::FieldGetter<GAnimation, float, false>::Signature) & GAnimation::m_duration,
			(meta::FieldSetter<GAnimation, float, false>::Signature) & GAnimation::m_duration, {});

		type.AddField<GAnimation, Array<Channel>>("m_channels",
			(meta::FieldGetter<GAnimation, Array<Channel>, false>::Signature) & GAnimation::m_channels,
			(meta::FieldSetter<GAnimation, Array<Channel>, false>::Signature) & GAnimation::m_channels, {});

		type.AddField<GAnimation, Array<BoneInfo>>("m_boneInfoArray",
			(meta::FieldGetter<GAnimation, Array<BoneInfo>, false>::Signature) & GAnimation::m_boneInfoArray,
			(meta::FieldSetter<GAnimation, Array<BoneInfo>, false>::Signature) & GAnimation::m_boneInfoArray, {});
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
		for (int32_t i = 0; i < m_boneInfoArray.size(); ++i)
		{
			m_boneInfos.insert({ m_boneInfoArray[i].m_boneName, m_boneInfoArray[i] });
		}
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
		GAnimation* animation = new GAnimation();
		animation->m_channels = m_channels;
		animation->m_boneInfoArray = m_boneInfoArray;
		animation->m_ticksPerSecond = m_ticksPerSecond;
		animation->m_duration = m_duration;
		animation->m_boneInfos = m_boneInfos;
		return animation;
	}

	void GAnimation::OnSerialize(nlohmann::json& output) const
	{

	}

	void GAnimation::OnDeserialize(const nlohmann::json& input)
	{
		
	}

}