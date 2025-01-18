#pragma once

#include "Component.h"
#include <Core/Math/MyMath.h>

namespace GuGu {
	class LightComponent : public Component
	{
	public:
		LightComponent();

		virtual ~LightComponent();

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;
	
		math::float4 m_lightPosition;

		math::float4 m_lightColor;
	};
}

