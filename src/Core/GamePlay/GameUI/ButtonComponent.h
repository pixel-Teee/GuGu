#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Renderer/Color.h>

namespace GuGu {
	struct AssetData;
	class GTexture;
	class ButtonComponent : public UIComponent
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		ButtonComponent();

		virtual ~ButtonComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		void setTextureAsset(const std::shared_ptr<AssetData> inAssetData);

		std::shared_ptr<GTexture> getTexture() const;

		std::shared_ptr<AssetData> getTextureAsset() const;

		Color getNormalColor() const;
		Color& getNormalColor();
		void setNormalColor(Color inColor);

		Color getHoveredColor() const;
		Color& getHoveredColor();
		void setHoveredColor(Color inColor);

		Color getPressedColor() const;
		Color& getPressedColor();
		void setPressedColor(Color inColor);

		Color getDisabledColor() const;
		Color& getDisabledColor();
		void setDisabledColor(Color inColor);

		virtual std::shared_ptr<UIDrawInfo> generateUIDrawInformation(bool bFlip = false) override;

		virtual math::float2 getDesiredSize() const override;

		virtual void onPointerDown(UIPointerData pointerData);

		virtual void onPointerUp(UIPointerData pointerData);
	private:
		//transient state
		bool m_bIsPressed;

		bool m_bIsHovered;

		//button style
		std::shared_ptr<AssetData> m_buttonImage;

		Color m_normalColor;

		Color m_hoveredColor;

		Color m_pressedColor;

		Color m_disabledColor;
	};
}