#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Renderer/Color.h>
#include <nvrhi.h>

namespace GuGu {
	struct AssetData;
	struct UIDrawInfo;
	class GFont;
	class TextComponent : public UIComponent
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		TextComponent();

		virtual ~TextComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual std::shared_ptr<UIDrawInfo> generateUIDrawInformation(bool bFlip = false) override;

		virtual math::float2 getDesiredSize() const override;

		Color getColor() const;
		Color& getColor();
		void setColor(Color& inColor);

		float getFontPoint() const;
		float& getFontPoint();
		void setFontPoint(float inSize);

		std::shared_ptr<AssetData> getFontAsset() const;
		void setFontAsset(const std::shared_ptr<AssetData> inAssetData);
		std::shared_ptr<GFont> getFont() const;

		GuGuUtf8Str getText() const;
		GuGuUtf8Str& getText();
		void setText(const GuGuUtf8Str& inText);
	private:
		//text color
		Color m_color;

		GuGuUtf8Str m_text;

		//text asset
		std::shared_ptr<AssetData> m_fontAsset;

		float m_fontPoint;//font size
	};
}