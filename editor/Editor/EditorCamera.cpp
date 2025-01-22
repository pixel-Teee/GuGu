#include <pch.h>

#include "EditorCamera.h"
#include <Core/UI/Events.h>
#include <Core/UI/Viewport.h>
#include <Core/GamePlay/InputManager.h>
#include <Core/Timer.h>
#include <Application/Application.h>
#include <Core/GamePlay/World.h>

namespace GuGu {
	EditorCamera::EditorCamera(std::shared_ptr<ViewportWidget> inViewportWidget)
		: m_viewportWidget(inViewportWidget)
	{
		m_fov = 45.0f;
		m_width = 1280.0f;
		m_height = 920.0f;
	}
	EditorCamera::~EditorCamera()
	{
	}
	bool EditorCamera::move(float fElapsedTimeSecond)
	{
		m_moveOffset = math::float3(0, 0, 0);

		if (InputManager::getInputManager().isMouseDown(Keys::MiddleMouseButton) && InputManager::getInputManager().isKeyDown(Keys::LeftShift))
		{
			math::float2 mouseDelta = InputManager::getInputManager().getMouseDelta();

			math::float2 direction = mouseDelta.x * math::float3(-1.0f, 0.0f, 0.0) + mouseDelta.y * math::float3(0.0f, 1.0f, 0.0);

			//GuGu_LOGD("(%f, %f)", mouseDelta.x, mouseDelta.y);

			m_moveOffset += math::float3(direction, 0.0f) * m_moveSpeed;
		}

		if (m_moveOffset.x != 0 || m_moveOffset.y != 0 || m_moveOffset.z != 0)
		{
			m_moveTarget = m_position + m_moveOffset; //移动到的指定位置
			//平滑移动
			//m_position = m_moveTarget;
			m_position = math::lerp(m_position, m_moveTarget, fElapsedTimeSecond * m_smoothMoveSpeed);
			return true;
		}
		return false;
	}
	bool EditorCamera::zoom(float fElapsedTimeSecond)
	{
		//m_moveOffset = math::float3(0, 0, 0);
		//
		float wheelDelta = InputManager::getInputManager().getWheelDelta();
		//
		//math::float3 direction = m_forward * wheelDelta;
		//
		//m_moveOffset += direction * m_zoomSpeed;

		if (wheelDelta != 0.0f)
		{
			m_fov -= wheelDelta;
			if(m_fov < 1.0f)
				m_fov = 1.0f;
			if(m_fov > 45.0f)
				m_fov = 45.0f;
			return true;
		}
		return false;
	}
	bool EditorCamera::rotate(float fElapsedTimeSecond)
	{
		if (InputManager::getInputManager().isMouseDown(Keys::MiddleMouseButton) && !InputManager::getInputManager().isKeyDown(Keys::LeftShift))
		{
			math::float2 mouseDelta = InputManager::getInputManager().getMouseDelta();

			m_yaw += mouseDelta.x * fElapsedTimeSecond;
			m_pitch += (-mouseDelta.y) * fElapsedTimeSecond;

			if(m_pitch > 89.0f)
				m_pitch = 89.0f;
			if(m_pitch < -89.0f)
				m_pitch = -89.0f;

			//GuGu_LOGD("%s", "rotate");
			bool isLeftShiftDown = InputManager::getInputManager().isKeyDown(Keys::LeftShift);
			math::matrix pichRollMatrix = math::affineToHomogeneous(math::yawPitchRoll(m_yaw, m_pitch, 0.0f));
			math::float3 newFront = math::float4(1.0f, 1.0f, 1.0f, 0.0f) * pichRollMatrix;

			m_forward = math::normalize(newFront);
			return true;
		}
		return false;
	}
	void EditorCamera::update(float fElapsedTimeSecond)
	{
		if(!move(fElapsedTimeSecond))
		{
			if(!zoom(fElapsedTimeSecond))
				rotate(fElapsedTimeSecond);
		}
		//move(fElapsedTimeSecond);
		//zoom(fElapsedTimeSecond);
		//rotate(fElapsedTimeSecond);

		m_forward = math::normalize(m_forward);
		m_right = math::normalize(math::cross(math::float3(0.0f, 1.0f, 0.0f), m_forward));
		m_up = math::normalize(math::cross(m_forward, m_right));

		//World::getWorld()->setWorldToViewMatrix(getWorldToViewMatrix());
		//World::getWorld()->setCamPos(m_position);
		//World::getWorld()->setFov(m_fov);

		if (m_viewportWidget.lock())
			m_viewportWidget.lock()->setRenderTarget(m_renderTarget);
	}
	math::affine3 EditorCamera::getWorldToViewMatrix() const
	{
		math::affine3 worldToView = math::affine3::from_cols(m_right, m_up, m_forward, -m_position);
		return worldToView;
	}
	float EditorCamera::getAspectRatio() const
	{
		return m_width / m_height;
	}
	math::float3 EditorCamera::getCamPos() const
	{
		return m_position;
	}
	float EditorCamera::getFov() const
	{
		return m_fov;
	}
	void EditorCamera::resizeViewport(int32_t width, int32_t height)
	{
		m_width = width;
		m_height = height;
	}
	math::float2 EditorCamera::getViewportSize() const
	{
		return math::float2(m_width, m_height);
	}
	void EditorCamera::setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer)
	{
		m_renderTarget = viewportRenderTarget;
		m_depthTarget = depthRenderTarget;
		m_frameBuffer = frameBuffer;
	}
	nvrhi::TextureHandle EditorCamera::getRenderTarget() const
	{
		return m_renderTarget;
	}
	nvrhi::TextureHandle EditorCamera::getDepthTarget() const
	{
		return m_depthTarget;
	}
	nvrhi::FramebufferHandle EditorCamera::getFramebuffer() const
	{
		return m_frameBuffer;
	}
	math::float2 EditorCamera::getRenderTargetSize() const
	{
		return math::float2(m_renderTarget->getDesc().width, m_renderTarget->getDesc().height);
	}
}