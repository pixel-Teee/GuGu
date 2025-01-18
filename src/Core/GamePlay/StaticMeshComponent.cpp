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
}