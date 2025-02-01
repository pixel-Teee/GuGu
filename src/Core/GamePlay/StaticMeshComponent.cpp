#include <pch.h>

#include "StaticMeshComponent.h"
#include <Core/AssetManager/AssetData.h>

namespace GuGu {
	StaticMeshComponent::StaticMeshComponent()
	{
		m_staticMeshAsset = std::make_shared<AssetData>();
	}
	StaticMeshComponent::~StaticMeshComponent()
	{
	}

	meta::Object* StaticMeshComponent::Clone(void) const
	{
		StaticMeshComponent* staticMeshComponent = new StaticMeshComponent();
		staticMeshComponent->m_staticMeshAsset = m_staticMeshAsset;
		return staticMeshComponent;
	}

	void StaticMeshComponent::Update(float fElapsedTimeSeconds)
	{
	}
	void StaticMeshComponent::setGStaticMesh(const GStaticMesh& gStaticMesh)
	{
		//copy static mesh
		m_staticMeshAsset->m_loadedResource = std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gStaticMesh.Clone()));
	}

	void StaticMeshComponent::setGStaticMesh(const std::shared_ptr<GStaticMesh>& gStaticMesh)
	{
		m_staticMeshAsset->m_loadedResource = gStaticMesh;
	}

	void StaticMeshComponent::setGStaticMesh(const std::shared_ptr<AssetData>& inAssetData)
	{
		m_staticMeshAsset = inAssetData;
	}

	std::shared_ptr<GStaticMesh> StaticMeshComponent::getStaticMesh() const
	{
		return std::static_pointer_cast<GStaticMesh>(m_staticMeshAsset->m_loadedResource);
	}
	std::shared_ptr<GStaticMesh> StaticMeshComponent::getStaticMesh()
	{
		return std::static_pointer_cast<GStaticMesh>(m_staticMeshAsset->m_loadedResource);
	}
	std::shared_ptr<AssetData> StaticMeshComponent::getStaticMeshAsset() const
	{
		return m_staticMeshAsset;
	}
	void StaticMeshComponent::setStaticMeshAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_staticMeshAsset = inAssetData;
	}
	meta::Type StaticMeshComponent::GetType() const
	{
		return typeof(StaticMeshComponent);
	}
}