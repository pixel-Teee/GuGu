#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Renderer/Color.h>
#include <nvrhi.h>

namespace GuGu {
	struct AssetData;
	struct UIDrawInfo;
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

	private:
		//text color
		Color m_color;

		GuGuUtf8Str m_text;
	};
}