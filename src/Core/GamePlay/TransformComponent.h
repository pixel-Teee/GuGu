#pragma once

#include "Component.h"

namespace GuGu {
	//transform component
	class TransformComponent : public Component
	{
	public:
		TransformComponent();

		virtual ~TransformComponent();

		void Update(float fElapsedTimeSeconds) override;

	private:

	};
}