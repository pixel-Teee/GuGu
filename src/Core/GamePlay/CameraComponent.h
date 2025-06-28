#pragma once

#include "Component.h"
#include <Renderer/nvrhi.h>

namespace GuGu {
	class CameraComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		CameraComponent();

		virtual ~CameraComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		float getFov() const;

		void setFov(float fov);

		float getNearPlane() const;

		void setNearPlane(float nearPlane);

		float getFarPlane() const;

		void setFarPlane(float farPlane);

		//camera frustum
		nvrhi::BufferHandle m_debugCameraFrustumVertexBuffer;
		nvrhi::BufferHandle m_debugCameraFrustumIndexBuffer;
	private:

		float m_fov;
		float m_nearPlane = 0.01f;
		float m_farPlane = 20.0f;
	};
}