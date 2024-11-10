#include <pch.h>

#include "Level.h"

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
	const std::vector<std::shared_ptr<GameObject>>& Level::getGameObjects()
	{
		return m_objects;
	}
	void Level::addGameObject(std::shared_ptr<GameObject> inGameObject)
	{
		m_objects.push_back(inGameObject);
	}
}