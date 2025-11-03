#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/Math/MyMath.h>
#include <Renderer/Color.h>
#include <Renderer/nvrhi.h>

namespace GuGu {
	struct AssetData;
	class GTexture;
	class CubeComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		CubeComponent();

		virtual ~CubeComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		//void setTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		//std::shared_ptr<GTexture> getTexture() const;
		//
		//std::shared_ptr<AssetData> getTextureAsset() const;

		//Color m_color;

		void setLeftTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getLeftTexture() const;

		std::shared_ptr<AssetData> getLeftTextureAsset() const;

		void setRightTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getRightTexture() const;

		std::shared_ptr<AssetData> getRightTextureAsset() const;

		void setFrontTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getFrontTexture() const;

		std::shared_ptr<AssetData> getFrontTextureAsset() const;


		void setBackTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getBackTexture() const;

		std::shared_ptr<AssetData> getBackTextureAsset() const;


		void setTopTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getTopTexture() const;

		std::shared_ptr<AssetData> getTopTextureAsset() const;


		void setBottomTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getBottomTexture() const;

		std::shared_ptr<AssetData> getBottomTextureAsset() const;

		nvrhi::TextureHandle m_textureHandle;
	private:
		std::shared_ptr<AssetData> m_left;
		std::shared_ptr<AssetData> m_right;
		std::shared_ptr<AssetData> m_front;
		std::shared_ptr<AssetData> m_back;
		std::shared_ptr<AssetData> m_top;
		std::shared_ptr<AssetData> m_bottom;
	};
}

