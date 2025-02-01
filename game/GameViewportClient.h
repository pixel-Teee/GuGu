#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GamePlay/ViewportClient.h>

namespace GuGu {
	struct KeyEvent;
	class GameObject;
	class GStaticMesh;
	class GameViewportClient : public ViewportClient
	{
	public:
		GameViewportClient(std::shared_ptr<ViewportWidget> inViewportWidget);

		virtual ~GameViewportClient();

		virtual void update(float fElapsedTimeSecond) override;


		math::float4x4 getWorldToViewMatrix() const override;


		math::float4x4 getPespectiveMatrix() const override;


		math::float3 getCamPos() const override;


		float getFov() const override;


		void resizeViewport(int32_t width, int32_t height) override;


		math::float2 getViewportSize() const override;


		void setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer) override;


		nvrhi::TextureHandle getRenderTarget() const override;


		nvrhi::TextureHandle getDepthTarget() const override;


		nvrhi::FramebufferHandle getFramebuffer() const override;


		math::float2 getRenderTargetSize() const override;


		float getAspectRatio() const override;


		float getNearPlane() const override;


		float getFarPlane() const override;


		const std::vector<std::shared_ptr<GStaticMesh>>& getGizmos() const override;


		std::vector<std::shared_ptr<GStaticMesh>>& getGizmos() override;


		math::float4 getGizmosColor(uint32_t index) const override;


		bool gizmosIsVisible() const override;


		std::shared_ptr<GameObject> getSelectedItems() const override;


		std::vector<uint32_t> getGizmosRenderSort() const override;

	private:
		std::weak_ptr<ViewportWidget> m_viewportWidget;
	};
}