#include <pch.h>

#include "CameraComponent.h"

namespace GuGu {

	CameraComponent::CameraComponent()
	{
		m_fov = 60.0f;
		
	}

	CameraComponent::~CameraComponent()
	{

	}

	meta::Object* CameraComponent::Clone(void) const
	{
		return nullptr;
	}

	void CameraComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type CameraComponent::GetType() const
	{
		return typeof(CameraComponent);
	}

	float CameraComponent::getFov() const
	{
		return m_fov;
	}

	void CameraComponent::setFov(float fov)
	{
		m_fov = fov;
	}

	float CameraComponent::getNearPlane() const
	{
		return m_nearPlane;
	}

	void CameraComponent::setNearPlane(float nearPlane)
	{
		m_nearPlane = nearPlane;
	}

	float CameraComponent::getFarPlane() const
	{
		return m_farPlane;
	}

	void CameraComponent::setFarPlane(float farPlane)
	{
		m_farPlane = farPlane;
	}

}

