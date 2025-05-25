#pragma once

#include "Component.h"
#include <Core/Math/MyMath.h>
#include <Renderer/Color.h>

namespace GuGu {
	class LightComponent : public Component
	{
	public:
		DECLARE_INITIAL
		LightComponent();

		virtual ~LightComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;
	
		math::float4 m_lightPosition;

		Color m_lightColor;
	};
}

