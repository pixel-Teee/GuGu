#pragma once

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>
#include <functional>

namespace GuGu {
	class GStaticMesh;
	class GameObject;
	class ViewportWidget;
	class ViewportClient
	{
	public:
		enum ViewportState
		{
			Editor,
			Runtime
		};

		enum Gizmos
		{
			Move,
			Rotation,
			Scale
		};
		ViewportClient();

		virtual ~ViewportClient();

		virtual ViewportState getViewportState() const = 0;

		virtual void setViewportState(ViewportState state) = 0;

		virtual math::float4x4 getWorldToViewMatrix() const = 0;

		virtual math::float4x4 getPespectiveMatrix() const = 0;

		virtual math::float3 getCamPos() const = 0;

		virtual float getFov() const = 0;

		virtual void update(const float inDeltaTime) = 0;

		virtual void resizeViewport(int32_t width, int32_t height) = 0;

		virtual math::float2 getViewportSize() const = 0;

		virtual void setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer)= 0;

		virtual nvrhi::TextureHandle getRenderTarget() const = 0;

		virtual nvrhi::TextureHandle getDepthTarget() const = 0;

		virtual nvrhi::FramebufferHandle getFramebuffer() const = 0;

		virtual math::float2 getRenderTargetSize() const = 0;

		virtual float getAspectRatio() const = 0;

		virtual float getNearPlane() const = 0;

		virtual float getFarPlane() const = 0;

		virtual const std::vector<std::shared_ptr<GStaticMesh>>& getGizmos() const = 0;

		virtual std::vector<std::shared_ptr<GStaticMesh>>& getGizmos() = 0;

		virtual math::float4 getGizmosColor(uint32_t index) const = 0;

		virtual bool gizmosIsVisible() const = 0;

		virtual std::shared_ptr<GameObject> getSelectedItems() const = 0;

		virtual std::vector<uint32_t> getGizmosRenderSort() const = 0;

		virtual float getScreenScaleCompensation(math::float3 objWorldPos) const = 0;

		virtual Gizmos getCurrentGizmosType() const = 0;

		//------暂时放在这里------
		using GameObjectSelectionChangedEvent = std::function<void(const std::vector<GameObject*>&, bool)>;
		virtual GameObjectSelectionChangedEvent& onGameObjectSelectionChanged() { return m_gameObjectSelectionChangedEvent; }

		virtual void broadcastGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh);
		//------暂时放在这里------
	private:
		GameObjectSelectionChangedEvent m_gameObjectSelectionChangedEvent;
	};
}