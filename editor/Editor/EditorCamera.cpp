#include <pch.h>

#include "EditorCamera.h"
#include <Core/UI/Events.h>
#include <Core/UI/Viewport.h>
#include <Core/GamePlay/InputManager.h>
#include <Core/Timer.h>
#include <Application/Application.h>
#include <Core/GamePlay/World.h>
#include <Core/Collision/Collision3D.h>
#include <Core/GamePlay/Level.h>
#include <Core/Model/GeometryHelper.h>

namespace GuGu {
	EditorCamera::EditorCamera(std::shared_ptr<ViewportWidget> inViewportWidget)
		: m_viewportWidget(inViewportWidget)
	{
		m_fov = 60.0f;
		m_width = 1280.0f;
		m_height = 920.0f;
		m_yaw = 0.0f;
		m_pitch = 0;
		m_bShowGizmos = false;
		updateView();
		makeMoveGizmos();
	}
	EditorCamera::~EditorCamera()
	{
	}
	void EditorCamera::move(float fElapsedTimeSecond)
	{
		m_moveOffset = math::float3(0, 0, 0);

		if (InputManager::getInputManager().isMouseDown(Keys::MiddleMouseButton) && InputManager::getInputManager().isKeyDown(Keys::LeftShift))
		{
			math::float2 mouseDelta = InputManager::getInputManager().getMouseDelta();
			math::float2 speedDelta = moveSpeed();
			m_focalPoint += -getRightDirection() * mouseDelta.x * speedDelta.x * m_distance * fElapsedTimeSecond;
			m_focalPoint += getUpDirection() * mouseDelta.y * speedDelta.y * m_distance * fElapsedTimeSecond;
		}
	}
	void EditorCamera::zoom(float fElapsedTimeSecond)
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
			//GuGu_LOGD("%f", wheelDelta);
			m_distance -= fElapsedTimeSecond * wheelDelta * 10;
			if (m_distance < 0.01f)
			{
				m_focalPoint += getForwardDirection();
				m_distance = 0.01f;
			}
		}
	}
	void EditorCamera::rotate(float fElapsedTimeSecond)
	{
		if (InputManager::getInputManager().isMouseDown(Keys::MiddleMouseButton) && !InputManager::getInputManager().isKeyDown(Keys::LeftShift))
		{
			math::float2 delta = InputManager::getInputManager().getMouseDelta();
			float yawSign = getUpDirection().y < 0.0f ? -1.0f : 1.0f;
			m_yaw += yawSign * delta.x * 0.05f * fElapsedTimeSecond;
			m_pitch += delta.y * 0.05f * fElapsedTimeSecond;
		}
	}
	void EditorCamera::update(float fElapsedTimeSecond)
	{
		move(fElapsedTimeSecond);
		rotate(fElapsedTimeSecond);
		zoom(fElapsedTimeSecond);
		gizmos(fElapsedTimeSecond);
		updateView();

		//m_forward = math::normalize(m_forward);
		//m_right = math::normalize(math::cross(m_up, m_forward));
		//m_up = math::normalize(math::cross(m_forward, m_right));

		if (m_viewportWidget.lock())
			m_viewportWidget.lock()->setRenderTarget(m_renderTarget);
	}
	math::float4x4 EditorCamera::getWorldToViewMatrix() const
	{
		//math::affine3 worldToView = math::affine3::from_cols(m_right, m_up, m_forward, -m_position);
		//return worldToView;

		math::quat orientation = getOrientation();
		math::affine3 worldToView = orientation.toAffine() * math::translation(m_position);	
		return math::inverse(math::affineToHomogeneous(worldToView));
	}
	math::float4x4 EditorCamera::getPespectiveMatrix() const
	{
		math::matrix perspectiveMatrix = math::perspProjD3DStyle(getFov(),
			getAspectRatio(), getNearPlane(), getFarPlane()
		);
		return perspectiveMatrix;
	}
	float EditorCamera::getAspectRatio() const
	{
		return (float)m_width / m_height;
	}
	math::float3 EditorCamera::getCamPos() const
	{
		return m_position;
	}
	float EditorCamera::getFov() const
	{
		return math::radians(m_fov);
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
	float EditorCamera::getNearPlane() const
	{
		return m_nearPlane;
	}
	float EditorCamera::getFarPlane() const
	{
		return m_farPlane;
	}
	void EditorCamera::updateView()
	{
		m_position = calculatePosition();
	}
	math::float3 EditorCamera::calculatePosition() const
	{
		return m_focalPoint - getForwardDirection() * m_distance;
	}
	math::float3 EditorCamera::getForwardDirection() const
	{
		return math::applyQuat(getOrientation(), math::float3(0.0f, 0.0f, 1.0f));
	}
	math::quat EditorCamera::getOrientation() const
	{
		return math::rotationQuat(math::float3(m_pitch, m_yaw, 0.0f));
	}
	math::float3 EditorCamera::getRightDirection() const
	{
		return math::applyQuat(getOrientation(), math::float3(1.0f, 0.0f, 0.0f));
	}
	math::float3 EditorCamera::getUpDirection() const
	{
		return math::applyQuat(getOrientation(), math::float3(0.0f, 1.0f, 0.0f));
	}
	math::float2 EditorCamera::moveSpeed()
	{
		float x = std::min(m_width / 1000.0f, 2.4f);//max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_height / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}
	float EditorCamera::zoomSpeed()
	{
		float distance = m_distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);//max speed = 100
		return speed;
	}
	void EditorCamera::gizmos(float fElapsedTimeSecond)
	{
		if (m_bShowGizmos)
		{
			//和gizmos先做碰撞检测

		}

		if (InputManager::getInputManager().isMouseDown(Keys::LeftMouseButton))
		{
			math::float2 mousePosition = InputManager::getInputManager().getMousePosition();
			//GuGu_LOGD("%f %f", mousePosition.x, mousePosition.y);
			std::shared_ptr<GameObject> pickedItem = Collision3D::pick(mousePosition.x, mousePosition.y, m_width, m_height, getPespectiveMatrix(), getWorldToViewMatrix(), World::getWorld()->getCurrentLevel()->getGameObjects(), m_debugDrawWorldPos);
			if (pickedItem)
			{
				m_bShowGizmos = true;
				m_pickedGameObject = pickedItem;
			}
			else
			{
				m_bShowGizmos = false;
				m_pickedGameObject = nullptr;
			}

			debugPitchAndYaw();
		}

	}
	const std::vector<std::shared_ptr<GStaticMesh>>& EditorCamera::getGizmos() const
	{
		return m_moveGizmos;
	}
	std::vector<std::shared_ptr<GStaticMesh>>& EditorCamera::getGizmos()
	{
		return m_moveGizmos;
	}
	bool EditorCamera::gizmosIsVisible() const
	{
		return m_bShowGizmos;
	}
	std::shared_ptr<GameObject> EditorCamera::getSelectedItems() const
	{
		return m_pickedGameObject;
	}
	void EditorCamera::debugPitchAndYaw()
	{
		float pitchRad = math::radians(m_pitch);
		float yawRad = math::radians(m_yaw);

		float cosPitch = std::cos(pitchRad);
		float sinPitch = std::sin(pitchRad);
		float cosYaw = std::cos(yawRad);
		float sinYaw = std::sin(yawRad);

		math::float3x3 rotationMatrix = math::float3x3(
			cosYaw, 0, sinYaw,
			sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw,
			-cosPitch * sinYaw, sinPitch, cosPitch * cosYaw
		);

		math::float3 forward = math::float3(0, 0, 1);
		math::float3 right = math::float3(1, 0, 0);
		math::float3 up = math::float3(0, 1, 0);

		forward = math::normalize(forward * rotationMatrix);
		right = math::normalize(right * rotationMatrix);
		up = math::normalize(up * rotationMatrix);

		GuGu_LOGD("forward:(%f, %f, %f), right:(%f, %f, %f), up:(%f, %f, %f)", forward.x, forward.y, forward.z,
			right.x, right.y, right.z,
			up.x, up.y, up.z);
	}

	void EditorCamera::makeMoveGizmos()
	{
		//先制作一根箭头

		//绿色
		GStaticMesh gstaticMesh = GeometryHelper::createCylinder(0.2f, 0.05f, 0.2f, 16, 4);
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));//垂直于xy平面
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.4f, 16, 4);
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
	}
}