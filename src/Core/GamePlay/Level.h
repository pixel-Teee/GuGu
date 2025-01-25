#pragma once

#include <Core/Reflection/Array.h>
#include "GameObject.h"

namespace GuGu {
	class Level {
	public:
		Level();

		virtual ~Level();

		virtual void Update(float fElapsedTimeSeconds);

		const std::vector<std::shared_ptr<GameObject>>& getGameObjects();

		void addGameObject(std::shared_ptr<GameObject> inGameObject);
	private:
		Array<std::shared_ptr<GameObject>> m_objects; //all game objects
	};
}