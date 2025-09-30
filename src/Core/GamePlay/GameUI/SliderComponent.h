#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Renderer/Color.h>
#include <nvrhi.h>

namespace GuGu {
	class SliderComponent : public UIComponent
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		SliderComponent();

		virtual ~SliderComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual std::shared_ptr<UIDrawInfo> generateUIDrawInformation(bool bFlip = false) override;

		virtual math::float2 getDesiredSize() const override;

	private:

	};
}