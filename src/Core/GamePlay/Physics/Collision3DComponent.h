#pragma once

#include <Core/GamePlay/Component.h>

namespace GuGu {
	class Collision3DComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		Collision3DComponent();

		virtual ~Collision3DComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

	private:

	};
}