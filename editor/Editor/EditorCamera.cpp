#include <pch.h>

#include "EditorCamera.h"
#include <Core/UI/Events.h>
#include <Core/GamePlay/InputManager.h>
#include <Core/Timer.h>
#include <Application/Application.h>
#include <Core/GamePlay/World.h>

namespace GuGu {
	EditorCamera::EditorCamera()
	{
	}
	EditorCamera::~EditorCamera()
	{
	}
	void EditorCamera::move(float fElapsedTimeSecond)
	{
		m_moveOffset = math::float3(0, 0, 0);

		//if (InputManager::getInputManager().isMouseDown(Keys::LeftAlt))
		//{
		math::float3 direction = math::float3(0, 0, 0);
		if (InputManager::getInputManager().isKeyDown(Keys::W))
			direction = m_forward;
		else if (InputManager::getInputManager().isKeyDown(Keys::S))
			direction = -m_forward;
		else if (InputManager::getInputManager().isKeyDown(Keys::A))
			direction = m_right;
		else if (InputManager::getInputManager().isKeyDown(Keys::D))
			direction = -m_right;
		m_moveOffset += direction * m_moveSpeed;
		//}

		if (m_moveOffset.x != 0 || m_moveOffset.y != 0 || m_moveOffset.z != 0)
		{
			m_moveTarget = m_position + m_moveOffset; //移动到的指定位置
			//平滑移动
			m_position = math::lerp(m_position, m_moveTarget, fElapsedTimeSecond * m_smoothMoveSpeed);
		}
	}
	void EditorCamera::update(float fElapsedTimeSecond)
	{
		move(fElapsedTimeSecond);

		m_forward = math::normalize(math::float3(0.0f, 0.0f, 0.0f) - m_position);
		m_right = math::normalize(math::cross(math::float3(0.0f, 1.0f, 0.0f), m_forward));
		m_up = math::normalize(math::cross(m_forward, m_right));

		World::getWorld()->setWorldToViewMatrix(getWorldToViewMatrix());
		World::getWorld()->setCamPos(m_position);
	}
	math::affine3 EditorCamera::getWorldToViewMatrix()
	{
		math::affine3 worldToView = math::affine3::from_cols(m_right, m_up, m_forward, -m_position);
		return worldToView;
	}
}