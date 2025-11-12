#pragma once

#include "Component.h"
#include <Renderer/nvrhi.h>

namespace GuGu {
	struct AssetData;
	class GTexture;
	class GrassComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GrassComponent();

		virtual ~GrassComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual void PostLoad() override;

		math::float4 m_bottomColor;

		math::float4 m_middleColor;

		math::float4 m_topColor;
	};
}