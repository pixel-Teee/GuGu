#include <pch.h>

#include "MaterialComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>

namespace GuGu {
	MaterialComponent::MaterialComponent()
	{
		m_albedo = Color(1.0f, 0.8f, 1.0f, 1.0f);
		m_metallic = 0.05f;
		m_roughness = 0.1f;
		m_ao = 0.0f;

		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_albedoTexture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));
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
		materialComponent->m_albedoTexture = m_albedoTexture;
		return materialComponent;
	}
	void MaterialComponent::Update(float fElapsedTimeSeconds)
	{
	}
	meta::Type MaterialComponent::GetType() const
	{
		return meta::Type(typeof(MaterialComponent));
	}

	std::shared_ptr<GuGu::AssetData> MaterialComponent::getAlbeoTextureAsset() const
	{
		return m_albedoTexture;
	}

	void MaterialComponent::setAlbeoTextureAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_albedoTexture = inAssetData;
	}

	std::shared_ptr<GTexture> MaterialComponent::getAlbedoTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_albedoTexture->m_loadedResource);
	}

}