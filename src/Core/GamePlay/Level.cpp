#include <pch.h>

#include "Level.h"
#include "World.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuLevel()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Level");
		auto& type = db.types[id];
		meta::TypeInfo<Level>::Register(id, type, true, "64CB643F-2C9C-4857-AA01-4F6F995A4CBE");

		auto typeID = typeidof(Level);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Level>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<Level>();

			type.AddConstructor<Level, false, false>({});

			type.AddConstructor<Level, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			type.AddField<Level, Array<std::shared_ptr<GameObject>>>("m_objects",
				(meta::FieldGetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Level::getGameObjects,
				(meta::FieldSetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Level::setGameObjects, {});

			meta::TypeInfo<Level>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::Level>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Level>>::Register(id, type, false, "5078441C-57E0-4322-9294-11D02360F9C9");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::Level>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Level>>::Register(id, type, false, "A027395A-6B76-474C-89D3-1D56ACC8212F");
		}
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(Level)
		ADD_PRIORITY(GameObject)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuLevel)
	IMPLEMENT_INITIAL_END

	Level::Level()
	{
	}
	Level::~Level()
	{
	}
	void Level::Update(float fElapsedTimeSeconds)
	{
		//call object Update
		for (size_t i = 0; i < m_objects.size(); ++i)
		{
			m_objects[i]->Update(fElapsedTimeSeconds);
		}

		//m_objects[0]->getComponent<Component>();
	}

	void Level::PostLoad()
	{

	}

	meta::Type Level::GetType() const
	{
		return typeof(Level);
	}
	meta::Object* Level::Clone(void) const
	{
		Level* level = new Level();
		level->m_objects = m_objects;
		return level;
	}
	void Level::OnSerialize(nlohmann::json& output) const
	{
	}
	void Level::OnDeserialize(const nlohmann::json& input)
	{
	}
	const Array<std::shared_ptr<GameObject>>& Level::getGameObjects() const
	{
		return m_objects;
	}
	Array<std::shared_ptr<GameObject>>& Level::getGameObjects()
	{
		return m_objects;
	}
	void Level::setGameObjects(const Array<std::shared_ptr<GameObject>>& gameObjects)
	{
		m_objects = gameObjects;
	}
	void Level::addGameObject(std::shared_ptr<GameObject> inGameObject)
	{
		m_objects.push_back(inGameObject);

		if (!World::getWorld()->m_onLevelChanged.empty())
		{
			for (uint32_t i = 0; i < World::getWorld()->m_onLevelChanged.size(); ++i)
			{
				World::getWorld()->m_onLevelChanged[i]();
			}
		}
	}

	void Level::deleteGameObject(std::shared_ptr<GameObject> inGameObject)
	{
		//delete game object
		int32_t foundPos = -1;
		if (inGameObject != nullptr)
		{
			for (int32_t i = 0; i < m_objects.size(); ++i)
			{
				if (m_objects[i] == inGameObject)//delete
				{
					foundPos = i;
					break;
				}
			}
			if (foundPos != -1)
				m_objects.erase(m_objects.begin() + foundPos);

			//refresh
			if (!World::getWorld()->m_onLevelChanged.empty())
			{
				for (uint32_t i = 0; i < World::getWorld()->m_onLevelChanged.size(); ++i)
				{
					World::getWorld()->m_onLevelChanged[i]();
				}
			}
		}
	}

}