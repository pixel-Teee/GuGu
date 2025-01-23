#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GamePlay/ViewportClient.h>

namespace GuGu {
	struct KeyEvent;
	class EditorCamera : public ViewportClient
	{
	public:
		EditorCamera(std::shared_ptr<ViewportWidget> inViewportWidget);

		virtual ~EditorCamera();

		bool move(float fElapsedTimeSecond); //移动

		bool zoom(float fElapsedTimeSecond); //聚焦

		bool rotate(float fElapsedTimeSecond); //旋转

		virtual void update(float fElapsedTimeSecond) override;

		virtual math::affine3 getWorldToViewMatrix() const override;

		virtual float getAspectRatio() const override;

		virtual math::float3 getCamPos() const override;

		virtual float getFov() const override;

		virtual void resizeViewport(int32_t width, int32_t height) override;

		virtual math::float2 getViewportSize() const override;

		virtual void setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer) override;

		virtual nvrhi::TextureHandle getRenderTarget() const override;

		virtual nvrhi::TextureHandle getDepthTarget() const;

		virtual nvrhi::FramebufferHandle getFramebuffer() const;

		virtual math::float2 getRenderTargetSize() const override;

		virtual float getNearPlane() const override;

		virtual float getFarPlane() const override;
	private:

		//move
		math::float3 m_moveOffset = math::float3(0, 0, 0);
		float m_moveSpeed = 0.05f;
		float m_smoothMoveSpeed = 5.0f;

		//zoom
		float m_zoomSpeed = 2.0f;

		//camera
		math::float3 m_forward = math::float3(0, 0, 1.0); //摄像机的方向向量 
		math::float3 m_right = math::float3(1, 0, 0); //摄像机空间的x轴方向
		math::float3 m_up = math::float3(0.0, 1.0, 0.0);
		math::float3 m_position = math::float3(0, 0, -25);
		float m_fov;
		float m_yaw, m_pitch;
		float m_nearPlane = 1.0f;
		float m_farPlane = 1000.0f;

		math::float3 m_moveTarget = m_position;	

		uint32_t m_width, m_height;

		nvrhi::TextureHandle m_renderTarget;
		nvrhi::TextureHandle m_depthTarget;
		nvrhi::FramebufferHandle m_frameBuffer;
		std::weak_ptr<ViewportWidget> m_viewportWidget;
	};
}