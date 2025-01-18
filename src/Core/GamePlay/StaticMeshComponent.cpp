#include <pch.h>

#include "StaticMeshComponent.h"

namespace GuGu {
	StaticMeshComponent::StaticMeshComponent()
	{
	}
	StaticMeshComponent::~StaticMeshComponent()
	{
	}
	void StaticMeshComponent::Update(float fElapsedTimeSeconds)
	{
	}
	void StaticMeshComponent::setGStaticMesh(const GStaticMesh& gStaticMesh)
	{
		//copy static mesh
		m_staticMesh = std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gStaticMesh.Clone()));
	}
	std::shared_ptr<GStaticMesh> StaticMeshComponent::getStaticMesh() const
	{
		return m_staticMesh;
	}
	std::shared_ptr<GStaticMesh> StaticMeshComponent::getStaticMesh()
	{
		return m_staticMesh;
	}
	meta::Type StaticMeshComponent::GetType() const
	{
		return typeof(StaticMeshComponent);
	}
}