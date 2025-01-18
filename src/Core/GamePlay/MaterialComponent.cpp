#include <pch.h>

#include "MaterialComponent.h"

namespace GuGu {
	MaterialComponent::MaterialComponent()
	{
		m_albedo = math::float3(1.0f, 0.8f, 1.0f);
		m_metallic = 0.3f;
		m_roughness = 0.03f;
		m_ao = 0.0f;
	}
	MaterialComponent::~MaterialComponent()
	{
	}
	void MaterialComponent::Update(float fElapsedTimeSeconds)
	{
	}
	meta::Type MaterialComponent::GetType() const
	{
		return meta::Type(typeof(MaterialComponent));
	}
}