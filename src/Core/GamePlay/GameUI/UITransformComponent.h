#pragma once

#include <Core/GamePlay/TransformComponent.h>

namespace GuGu {
	class UITransformComponent : public TransformComponent
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		UITransformComponent();

		virtual ~UITransformComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;
	private:

	};
}