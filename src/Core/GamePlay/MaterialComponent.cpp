#include <pch.h>

#include "MaterialComponent.h"

namespace GuGu {
	MaterialComponent::MaterialComponent()
	{
		m_albedo = math::float3(1.0f, 0.8f, 1.0f);
		m_metallic = 0.05f;
		m_roughness = 0.1f;
		m_ao = 0.0f;
	}
	MaterialComponent::~MaterialComponent()
	{
	}
	meta::Object* MaterialComponent::Clone(void) const
	{
		MaterialComponent* materialComponent = new MaterialComponent();
		materialComponent->m_albedo = m_albedo;
		materialComponent->m_metallic = m_metallic;
		materialComponent->m_roughness = m_roughness;
		materialComponent->m_ao = m_ao;
		return materialComponent;
	}
	void MaterialComponent::Update(float fElapsedTimeSeconds)
	{
	}
	meta::Type MaterialComponent::GetType() const
	{
		return meta::Type(typeof(MaterialComponent));
	}
}