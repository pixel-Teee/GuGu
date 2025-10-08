#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/Math/MyMath.h>
#include <Renderer/Color.h>
#include <Renderer/nvrhi.h>

namespace GuGu {
	struct AssetData;
	class GTexture;
	class SpriteComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		SpriteComponent();

		virtual ~SpriteComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		void setTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getTexture() const;

		std::shared_ptr<AssetData> getTextureAsset() const;

		Color m_color;
	private:
		std::shared_ptr<AssetData> m_texture;

		nvrhi::BufferHandle m_bufferHandle;
	};
}

