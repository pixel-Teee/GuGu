#pragma once

#include "Reflection/Array.h"
#include "Reflection/Object.h"

namespace GuGu {
	class Level {
	public:
		Level();

		virtual ~Level();

		virtual void Update(float fElapsedTimeSeconds);

		const std::vector<std::shared_ptr<meta::Object>>& getGameObjects();
	private:
		Array<std::shared_ptr<meta::Object>> m_objects;
	};
}