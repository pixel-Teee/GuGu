#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Renderer/Color.h>
#include <nvrhi.h>

namespace GuGu {
	struct AssetData;
	struct UIDrawInfo;
	class GTexture;
	class ImageComponent : public UIComponent
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		ImageComponent();

		virtual ~ImageComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		void setTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getTexture() const;

		std::shared_ptr<AssetData> getTextureAsset() const;

		virtual std::shared_ptr<UIDrawInfo> generateUIDrawInformation(bool bFlip = false) override;

		//color
		Color m_color;
	private:
		std::shared_ptr<AssetData> m_texture;

		nvrhi::BufferHandle m_bufferHandle;
	};
}