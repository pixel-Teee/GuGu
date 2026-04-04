#include <pch.h>

#include "Cutscenes.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>
#include <Core/AssetManager/AssetManager.h>//asset data
#include <Core/Timer.h>
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/Level.h>
#include <Application/Application.h> //TODO:remove this
#include "Evaluator.h"
#include "Applier.h"

namespace GuGu {
	std::map<GuGuUtf8Str, std::shared_ptr<Applier>> Cutscenes::m_appliers;
	std::map<GuGuUtf8Str, std::shared_ptr<Evaluator>> Cutscenes::m_evaluators;

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

		type.AddStaticMethod<Cutscenes>("playCutscenes", &Cutscenes::playCutscenes, {});

		type.AddMethod("Update", &Cutscenes::Update, {});

		type.AddStaticMethod<Cutscenes>("getCutscenes", &Cutscenes::getCutscenes, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(Cutscenes)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCutscenes)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(Cutscenes)
		ADD_PRIORITY_FIELDS(AssetData)
		if (!ms_priority2.addPriorityThan(&trackDataPriority)) return 0; //track data
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCutscenesFields)
	IMPLEMENT_INITIAL_FIELDS_END
	Cutscenes::Cutscenes()
	{
		m_isRunning = false;
		m_startTime = 0.0f;
	}
	Cutscenes::~Cutscenes()
	{
	}

	void Cutscenes::PostLoad()
	{

	}

	void Cutscenes::Update(float fElapsedTimeSeconds)
	{
		if (m_isRunning)
		{
			//TODO:FIX THIS
			float currentTotalTime = Application::getApplication()->getTimer()->GetTotalTime();
			//run
			float currentTime = (currentTotalTime - m_startTime); //TODO:FIX THIS

			for (int32_t i = 0; i < m_trackDatas.size(); ++i)
			{
				const TrackData& trackData = m_trackDatas[i];

				//get evaluator
				std::shared_ptr<Evaluator> evaluator;
				if (m_evaluators.find(trackData.m_typeGuid) != m_evaluators.end())
				{
					evaluator = m_evaluators.find(trackData.m_typeGuid)->second;
				}
				else
				{
					evaluator = createEvaluator(trackData.m_typeGuid);
					m_evaluators[trackData.m_typeGuid] = evaluator;
				}
				//get applier
				std::shared_ptr<Applier> applier;
				if (m_appliers.find(trackData.m_typeGuid) != m_appliers.end())
				{
					applier = m_appliers.find(trackData.m_typeGuid)->second;
				}
				else
				{
					applier = createApplier(trackData.m_typeGuid);
					m_appliers[trackData.m_typeGuid] = applier;
				}

				//get value
				meta::Variant res = evaluator->Evaluate(trackData, currentTime);

				//get current level object
				std::optional<Section> currentSection = findSection(trackData, currentTime);

				if (currentSection.has_value())
				{
					Section section = currentSection.value();
					std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
					std::shared_ptr<meta::Object> currentObj = currentLevel->getGameObject(section.m_bindingObjectName);
					if (section.m_bindingObjectComponentTypeGuid != "")
					{
						meta::Type componetType = meta::Type::getType(section.m_bindingObjectComponentTypeGuid);
						currentObj = currentLevel->getComponent(section.m_bindingObjectName, componetType);
					}
					if (evaluator->getCurrentField() != "")
					{
						//set value
						applier->Apply(currentObj, res, evaluator->getCurrentField());
					}
				}			
			}
		}
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

	void Cutscenes::playCutscenes(std::shared_ptr<AssetData> inCutscenes)
	{
		std::shared_ptr<Cutscenes> cutscenes = std::static_pointer_cast<Cutscenes>(inCutscenes->m_loadedResource);
		cutscenes->setIsRunning(true);
		cutscenes->m_startTime = Application::getApplication()->getTimer()->GetTotalTime();
	}

	std::shared_ptr<Cutscenes> Cutscenes::getCutscenes(std::shared_ptr<AssetData> inCutscenes)
	{
		return std::static_pointer_cast<Cutscenes>(inCutscenes->m_loadedResource);
	}

	void Cutscenes::setIsRunning(bool inRunning)
	{
		m_isRunning = inRunning;
	}

	bool Cutscenes::getIsRunning()
	{
		return m_isRunning;
	}

}