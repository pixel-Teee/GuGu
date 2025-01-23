#include <pch.h>

#include "LightComponent.h"
#include <Core/Timer.h>

namespace GuGu {
	LightComponent::LightComponent()
	{
		m_lightPosition = math::float4(0.0f, 0.0f, -10.0f, 1.0f);
		m_lightColor = math::float4(200.0f, 200.0f, 900.0f, 1.0f);
	}
	LightComponent::~LightComponent()
	{
	}
	void LightComponent::Update(float fElapsedTimeSeconds)
	{
		//m_lightColor.x += std::cos(fElapsedTimeSeconds * 200.0);
	}
	meta::Type LightComponent::GetType() const
	{
		return meta::Type(typeof(LightComponent));
	}
}