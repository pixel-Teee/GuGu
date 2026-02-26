#include <pch.h>

#include "Cutscenes.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerGuGuCutscenes()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::Cutscenes");
			auto& type = db.types[id];
			meta::TypeInfo<Cutscenes>::Register(id, type, true, "9A54147C-B0E3-4EE9-8B56-64AC23508125");

			auto typeID = typeidof(Cutscenes);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Cutscenes>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<Cutscenes>();

				type.AddConstructor<Cutscenes, false, false>({});

				type.AddConstructor<Cutscenes, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<Cutscenes>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::Cutscenes>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<Cutscenes>>::Register(id, type, false, "C74EC3C4-082E-4D9A-B9F9-F26D1B2E7FE3");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::Cutscenes>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<Cutscenes>>::Register(id, type, false, "0A31B8D1-2E13-4CE4-ABD3-07BA4B2ED6DF");
			}
		}
		return true;
	}
	static bool registerGuGuCutscenesFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(Cutscenes).GetID()];

		type.AddField<Cutscenes, GuGuUtf8Str>("m_cutScenesName",
			(meta::FieldGetter<Cutscenes, GuGuUtf8Str&, true>::Signature) & Cutscenes::getCutscenesName,
			(meta::FieldSetter<Cutscenes, GuGuUtf8Str, true>::Signature) & Cutscenes::setCutscenesName, {});

		type.AddField<Cutscenes, float>("m_duration",
			(meta::FieldGetter<Cutscenes, float&, true>::Signature) & Cutscenes::getDuration,
			(meta::FieldSetter<Cutscenes, float, true>::Signature) & Cutscenes::setDuration, {});

		type.AddField<Cutscenes, float>("m_duration",
			(meta::FieldGetter<Cutscenes, float&, true>::Signature) & Cutscenes::getDuration,
			(meta::FieldSetter<Cutscenes, float, true>::Signature) & Cutscenes::setDuration, {});

		type.AddField<Cutscenes, int32_t>("m_ticksPerSecond",
			(meta::FieldGetter<Cutscenes, int32_t&, true>::Signature) & Cutscenes::getTicksPerSecond,
			(meta::FieldSetter<Cutscenes, int32_t, true>::Signature) & Cutscenes::setTicksPerSecond, {});

		type.AddField<Cutscenes, Array<TrackData>>("m_trackDatas",
			(meta::FieldGetter<Cutscenes, Array<TrackData>&, true>::Signature) & Cutscenes::getTrackData,
			(meta::FieldSetter<Cutscenes, Array<TrackData>, true>::Signature) & Cutscenes::setTrackData, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(Cutscenes)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCutscenes)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(Cutscenes)
		if (!ms_priority2.addPriorityThan(&trackDataPriority)) return 0; //track data
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCutscenesFields)
	IMPLEMENT_INITIAL_FIELDS_END
	Cutscenes::Cutscenes()
	{
	}
	Cutscenes::~Cutscenes()
	{
	}

	void Cutscenes::PostLoad()
	{

	}

	void Cutscenes::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type Cutscenes::GetType() const
	{
		return typeof(Cutscenes);
	}

	meta::Object* Cutscenes::Clone(void) const
	{
		//nothing
		return nullptr;
	}

	void Cutscenes::OnSerialize(nlohmann::json& output) const
	{

	}

	void Cutscenes::OnDeserialize(const nlohmann::json& input)
	{

	}

	GuGuUtf8Str& Cutscenes::getCutscenesName()
	{
		return m_cutScenesName;
	}

	void Cutscenes::setCutscenesName(const GuGuUtf8Str& inCutScenesName)
	{
		m_cutScenesName = inCutScenesName;
	}

	float& Cutscenes::getDuration()
	{
		return m_duration;
	}

	void Cutscenes::setDuration(float inDuration)
	{
		m_duration = inDuration;
	}

	int32_t& Cutscenes::getTicksPerSecond()
	{
		return m_ticksPerSecond;
	}

	void Cutscenes::setTicksPerSecond(int32_t inTicksPerSecond)
	{
		m_ticksPerSecond = inTicksPerSecond;
	}

	Array<TrackData>& Cutscenes::getTrackData()
	{
		return m_trackDatas;
	}

	void Cutscenes::setTrackData(const Array<TrackData>& inTrackData)
	{
		m_trackDatas = inTrackData;
	}

}