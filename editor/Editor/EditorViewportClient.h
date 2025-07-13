#pragma once

#include <Core/Math/MyMath.h>
#include <Core/UI/Reply.h>
#include <Core/GamePlay/ViewportClient.h>

namespace GuGu {
	struct KeyEvent;
	class GameObject;
	class GStaticMesh;
	class EditorViewportClient : public ViewportClient
	{
	public:
		EditorViewportClient(std::shared_ptr<ViewportWidget> inViewportWidget);

		virtual ~EditorViewportClient();

		void move(float fElapsedTimeSecond); //移动

		void zoom(float fElapsedTimeSecond); //聚焦

		void rotate(float fElapsedTimeSecond); //旋转

		virtual void update(float fElapsedTimeSecond) override;

		virtual math::float4x4 getWorldToViewMatrix() const override;

		virtual math::float4x4 getPespectiveMatrix() const override;

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

		void updateView();

		math::float3 calculatePosition() const;

		math::float3 getForwardDirection() const;

		math::quat getOrientation() const;

		math::float3 getRightDirection() const;

		math::float3 getUpDirection() const;

		math::float2 moveSpeed();

		float zoomSpeed();

		void gizmos(float fElapsedTimeSecond);

		virtual const std::vector<std::shared_ptr<GStaticMesh>>& getGizmos() const override;

		virtual std::vector<std::shared_ptr<GStaticMesh>>& getGizmos() override;

		virtual math::float4 getGizmosColor(uint32_t index) const override;

		virtual bool gizmosIsVisible() const override;

		virtual std::shared_ptr<GameObject> getSelectedItems() const override;

		void debugPitchAndYaw();

		virtual std::vector<uint32_t> getGizmosRenderSort() const override;//获取 gizmos 渲染顺序

		virtual ViewportState getViewportState() const override;

		virtual void setViewportState(ViewportState state) override;

		virtual float getScreenScaleCompensation(math::float3 objWorldPos) const override;

		virtual Gizmos getCurrentGizmosType() const override;

		//undo/redo
		virtual void modifyObject(std::shared_ptr<meta::Object> inObject) override;

		Reply handleUnhandledKeyDownEvent(const KeyEvent& inKeyEvent);

		virtual void setSelectItem(std::shared_ptr<GameObject> inGameObject) override;

		virtual math::float3 getDebugDrawPos() override;
	private:
		void makeGizmos();

		math::float3 m_focalPoint = math::float3(0.0f, 0.0f, 0.0f);//焦点在世界原点
		float m_distance = 2.0f;//focal point 和 distance 决定摄像机位置

		//move
		math::float3 m_moveOffset = math::float3(0, 0, 0);
		float m_moveSpeed = 0.05f;
		float m_smoothMoveSpeed = 5.0f;

		//zoom
		float m_zoomSpeed = 2.0f;

		//camera
		//math::float3 m_forward = math::float3(0, 0, 1.0); //摄像机的方向向量 
		//math::float3 m_right = math::float3(1, 0, 0); //摄像机空间的x轴方向
		//math::float3 m_up = math::float3(0.0, 1.0, 0.0);
		math::float3 m_position = math::float3(0, 0, -1);
		float m_fov;
		float m_yaw, m_pitch;
		float m_nearPlane = 0.01f;
		float m_farPlane = 1000.0f;

		math::float3 m_moveTarget = m_position;	

		uint32_t m_width, m_height;

		nvrhi::TextureHandle m_renderTarget;
		nvrhi::TextureHandle m_depthTarget;
		nvrhi::FramebufferHandle m_frameBuffer;
		std::weak_ptr<ViewportWidget> m_viewportWidget;

		bool m_bShowGizmos;
		std::shared_ptr<GameObject> m_pickedGameObject;

		Gizmos m_gizmos = Gizmos::Move;
		std::vector<std::shared_ptr<GStaticMesh>> m_currentGizmos;
		std::vector<std::shared_ptr<GStaticMesh>> m_moveGizmos;
		std::vector<math::float3> m_moveGizmoPos;
		uint32_t m_currentGizmosIndex = 0;
		uint32_t m_currentMousePositionX = 0, m_currentMousePositionY = 0;
		math::double3 m_pickedObjectDragStartWorldPosition = math::double3(0, 0, 0);
		math::float3 m_lastRayIntersectPoint = math::float3(0, 0, 0);
		math::float3 m_worldObjectAxis = math::float3(0, 0, 0);
		math::float3 m_planeNormal = math::float3(0, 0, 1);
		bool m_bdragging = false;

		std::vector<std::shared_ptr<GStaticMesh>> m_rotateGizmos;
		std::vector<math::float3> m_rotateGizmoPos;
		uint32_t m_currentRotateGizmosIndex = 0;
		float m_lastTheta = 0;

		std::vector<std::shared_ptr<GStaticMesh>> m_scaleGizmos;
		std::vector<math::float3> m_scaleGizmoPos;

		math::float4 m_debugDrawWorldPos = math::float4(1.0f);

		ViewportState m_viewportState;
	};
}