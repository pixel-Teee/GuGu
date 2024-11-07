#pragma once

#include "Reflection/Array.h"
#include "GameObject.h"

namespace GuGu {
	class Level {
	public:
		Level();

		virtual ~Level();

		virtual void Update(float fElapsedTimeSeconds);

		const std::vector<std::shared_ptr<GameObject>>& getGameObjects();
	private:
		Array<std::shared_ptr<GameObject>> m_objects;
	};
}