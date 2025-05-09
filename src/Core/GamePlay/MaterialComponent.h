#pragma once

#include "Component.h"
#include <Core/Math/MyMath.h>
#include <nvrhi.h>
#include <Renderer/Color.h>

namespace GuGu {
	class GTexture;
	struct AssetData;
	class MaterialComponent : public Component
	{
	public:
		MaterialComponent();

		virtual ~MaterialComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		std::shared_ptr<AssetData> getAlbeoTextureAsset() const;

		void setAlbeoTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getAlbedoTexture() const;

		Color m_albedo;
		float m_metallic;
		float m_roughness;
		float m_ao;
		nvrhi::BufferHandle m_bufferHandle;
		//nvrhi::TextureHandle m_albedoTextureHandle;

		std::shared_ptr<AssetData> m_albedoTexture;
	};
}