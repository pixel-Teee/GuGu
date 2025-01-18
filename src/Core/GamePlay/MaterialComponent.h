#pragma once

#include "Component.h"
#include <Core/Math/MyMath.h>
#include <nvrhi.h>

namespace GuGu {
	class MaterialComponent : public Component
	{
	public:
		MaterialComponent();

		virtual ~MaterialComponent();

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		math::float3 m_albedo;
		float m_metallic;
		float m_roughness;
		float m_ao;
		nvrhi::BufferHandle m_bufferHandle;
	};
}