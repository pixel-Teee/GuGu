#include <pch.h>

#include "ScriptComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerGuGuScriptComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::ScriptComponent");
		auto& type = db.types[id];
		meta::TypeInfo<ScriptComponent>::Register(id, type, true, "671B819A-F8C7-4921-8295-46BF1D2D857F");

		auto typeID = typeidof(ScriptComponent);

		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<ScriptComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<ScriptComponent>();

			type.AddConstructor<ScriptComponent, false, false>({});

			type.AddConstructor<ScriptComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<ScriptComponent>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::ScriptComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<ScriptComponent>>::Register(id, type, false, "7882B450-E4EA-4B40-BB06-DB928C1ADF41");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::ScriptComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<ScriptComponent>>::Register(id, type, false, "E2D97C02-377D-4200-8C8F-42BC113ED7BD");
		}

		return true;
	}

	static bool registerGuGuScriptComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(ScriptComponent).GetID()];

		type.AddField<ScriptComponent, GuGuUtf8Str>("m_scriptPath",
			(meta::FieldGetter<ScriptComponent, GuGuUtf8Str&, true>::Signature) & ScriptComponent::getScriptPath,
			(meta::FieldSetter<ScriptComponent, GuGuUtf8Str, true>::Signature) & ScriptComponent::setScriptPath, {});

		type.AddField<ScriptComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<ScriptComponent, std::weak_ptr<GameObject>&, true>::Signature) & ScriptComponent::getParentGameObject,
			(meta::FieldSetter<ScriptComponent, std::weak_ptr<GameObject>&, true>::Signature) & ScriptComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(ScriptComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuScriptComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(ScriptComponent)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuScriptComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	ScriptComponent::ScriptComponent()
	{

	}

	ScriptComponent::~ScriptComponent()
	{

	}

	meta::Object* ScriptComponent::Clone(void) const
	{
		ScriptComponent* scriptComponent = new ScriptComponent();
		scriptComponent->m_scriptPath = m_scriptPath;
		return scriptComponent;
	}

	void ScriptComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type ScriptComponent::GetType() const
	{
		return typeof(ScriptComponent);
	}

	GuGuUtf8Str ScriptComponent::getScriptPath() const
	{
		return m_scriptPath;
	}

	GuGuUtf8Str& ScriptComponent::getScriptPath()
	{
		return m_scriptPath;
	}

	void ScriptComponent::setScriptPath(const GuGuUtf8Str& inScriptPath)
	{
		m_scriptPath = inScriptPath;
	}

}