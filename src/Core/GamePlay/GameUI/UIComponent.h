#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/Math/MyMath.h>

#include "UIPointerData.h" //ui pointer data

namespace GuGu {
	struct UIDrawInfo;
	class UIComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		UIComponent();

		virtual ~UIComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual std::shared_ptr<UIDrawInfo> generateUIDrawInformation(bool bFlip = false);

		virtual math::float2 getDesiredSize() const;

		virtual void onPointerDown(UIPointerData pointerData);

		virtual void onPointerUp(UIPointerData pointerData);
	};
}