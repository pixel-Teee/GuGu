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
	}
	const std::vector<std::shared_ptr<meta::Object>>& Level::getGameObjects()
	{
		return m_objects;
	}
}