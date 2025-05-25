#pragma once

#include "Component.h"

namespace GuGu {
	class CameraComponent : public Component
	{
	public:
		DECLARE_INITIAL
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
	private:

		float m_fov;
		float m_nearPlane = 0.01f;
		float m_farPlane = 1000.0f;
	};
}