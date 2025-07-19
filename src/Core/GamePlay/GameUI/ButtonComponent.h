#pragma once

#include <Core/GamePlay/Component.h>

namespace GuGu {
	class ButtonComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		ButtonComponent();

		virtual ~ButtonComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;
	private:

	};
}