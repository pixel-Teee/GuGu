#include <pch.h>

#include "Level.h"
#include "World.h"

namespace GuGu {
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