#pragma once

#include <Core/GamePlay/Component.h>

namespace GuGu {

	//lua
	class ScriptComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		ScriptComponent();

		virtual ~ScriptComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;
	};
}