#include <pch.h>

#include "GameObjectLevelRef.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/World.h>

namespace GuGu {
	static bool registerGuGuGameObjectLevelRef()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GameObjectLevelRef");
			auto& type = db.types[id];
			meta::TypeInfo<GameObjectLevelRef>::Register(id, type, true, "7443F429-C6FC-43BE-9435-D03E1C8F9BBB");

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });
		}
		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GameObjectLevelRef>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GameObjectLevelRef>>::Register(id, type, false, "2943E43F-DBA1-4BCB-83D5-83BFC154C7A9");
		}
		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GameObjectLevelRef>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GameObjectLevelRef>>::Register(id, type, false, "E6E2776B-E20D-4230-979A-85DBEDB0E6C7");
		}
		return true;
	}

	static bool registerGuGuGameObjectLevelRefFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(GameObjectLevelRef).GetID()];

		type.AddField<GameObjectLevelRef, GuGuUtf8Str>("m_levelFilePath",
			(meta::FieldGetter<GameObjectLevelRef, GuGuUtf8Str, false>::Signature) & GameObjectLevelRef::m_levelFilePath,
			(meta::FieldSetter<GameObjectLevelRef, GuGuUtf8Str, false>::Signature) & GameObjectLevelRef::m_levelFilePath, {});

		type.AddField<GameObjectLevelRef, GuGuUtf8Str>("m_levelFileName",
			(meta::FieldGetter<GameObjectLevelRef, GuGuUtf8Str, false>::Signature) & GameObjectLevelRef::m_levelFileName,
			(meta::FieldSetter<GameObjectLevelRef, GuGuUtf8Str, false>::Signature) & GameObjectLevelRef::m_levelFileName, {});

		type.AddField<GameObjectLevelRef, GuGuUtf8Str>("m_gameObjectName",
			(meta::FieldGetter<GameObjectLevelRef, GuGuUtf8Str, false>::Signature) & GameObjectLevelRef::m_gameObjectName,
			(meta::FieldSetter<GameObjectLevelRef, GuGuUtf8Str, false>::Signature) & GameObjectLevelRef::m_gameObjectName, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(GameObjectLevelRef)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGameObjectLevelRef)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GameObjectLevelRef)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGameObjectLevelRefFields)
	IMPLEMENT_INITIAL_FIELDS_END

	void GameObjectLevelRef::Update(float fElapsedTimeSeconds)
	{
	}

	void GameObjectLevelRef::PostLoad()
	{

	}

	meta::Type GameObjectLevelRef::GetType() const
	{
		return typeof(GameObjectLevelRef);
	}
	meta::Object* GameObjectLevelRef::Clone(void) const
	{
		GameObjectLevelRef* gameObjectLevelRef = new GameObjectLevelRef();
		gameObjectLevelRef->m_levelFilePath = m_levelFilePath;
		gameObjectLevelRef->m_levelFileName = m_levelFileName;
		gameObjectLevelRef->m_gameObjectName = m_gameObjectName;
		return gameObjectLevelRef;
	}
	GameObjectLevelRef::GameObjectLevelRef(const GuGuUtf8Str& levelFilePath, const GuGuUtf8Str& levelFileName, const GuGuUtf8Str& gameObjectName)
		: m_levelFilePath(levelFilePath)
		, m_levelFileName(levelFileName)
		, m_gameObjectName(gameObjectName)
	{
	}

	GameObjectLevelRef::GameObjectLevelRef(const GameObjectLevelRef& rhs)
	{
		this->m_levelFilePath = m_levelFilePath;
		this->m_levelFileName = m_levelFileName;
		this->m_gameObjectName = m_gameObjectName;
	}

	GameObjectLevelRef& GameObjectLevelRef::operator=(const GameObjectLevelRef& rhs)
	{
		this->m_levelFilePath  =	rhs.m_levelFilePath;
		this->m_levelFileName  =	rhs.m_levelFileName;
		this->m_gameObjectName =	rhs.m_gameObjectName;
		return *this;
	}

	GuGuUtf8Str GameObjectLevelRef::toString() const
	{
		char buf[128];
		std::sprintf(buf, "(m_levelFilePath = %s, m_levelFileName = %s, m_gameObjectName = %s)", m_levelFilePath.getStr(), m_levelFileName.getStr(), m_gameObjectName.getStr());
		return buf;
	}

	std::shared_ptr<GameObject> GameObjectLevelRef::getGameObject()
	{
		//from current level to find game object
		std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
		return currentLevel->getGameObject(m_gameObjectName);
	}

}