#include <pch.h>

#include "GameViewportClient.h"

namespace GuGu {
	GameViewportClient::GameViewportClient(std::shared_ptr<ViewportWidget> inViewportWidget)
		: m_viewportWidget(inViewportWidget)
	{

	}
	GameViewportClient::~GameViewportClient()
	{

	}
	void GameViewportClient::update(float fElapsedTimeSecond)
	{

	}

	void GameViewportClient::resizeViewport(int32_t width, int32_t height)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float2 GameViewportClient::getViewportSize() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void GameViewportClient::setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::nvrhi::TextureHandle GameViewportClient::getRenderTarget() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::nvrhi::TextureHandle GameViewportClient::getDepthTarget() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::nvrhi::FramebufferHandle GameViewportClient::getFramebuffer() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float2 GameViewportClient::getRenderTargetSize() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	float GameViewportClient::getAspectRatio() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	float GameViewportClient::getNearPlane() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	float GameViewportClient::getFarPlane() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	const std::vector<std::shared_ptr<GuGu::GStaticMesh>>& GameViewportClient::getGizmos() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	std::vector<std::shared_ptr<GuGu::GStaticMesh>>& GameViewportClient::getGizmos()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float4 GameViewportClient::getGizmosColor(uint32_t index) const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool GameViewportClient::gizmosIsVisible() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	std::shared_ptr<GuGu::GameObject> GameViewportClient::getSelectedItems() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	std::vector<uint32_t> GameViewportClient::getGizmosRenderSort() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float4x4 GameViewportClient::getWorldToViewMatrix() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float4x4 GameViewportClient::getPespectiveMatrix() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float3 GameViewportClient::getCamPos() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	float GameViewportClient::getFov() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}