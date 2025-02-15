#include <pch.h>

#include "GameViewportClient.h"
#include <Core/UI/WindowWidget.h>
#include <Core/UI/NullWidget.h>
#include <Core/GamePlay/World.h>
#include <Core/AssetManager/AssetManager.h>
#include "LevelViewport.h"

namespace GuGu {
	GameViewportClient::GameViewportClient(std::shared_ptr<ViewportWidget> inViewportWidget)
		: m_viewportWidget(inViewportWidget)
	{
		m_width = 1280.0f;
		m_height = 920.0f;
	}
	GameViewportClient::~GameViewportClient()
	{

	}
	void GameViewportClient::update(float fElapsedTimeSecond)
	{
		if (m_viewportWidget.lock())
			m_viewportWidget.lock()->setRenderTarget(m_renderTarget);
	}

	void GameViewportClient::resizeViewport(int32_t width, int32_t height)
	{
		m_width = width;
		m_height = height;
	}

	math::float2 GameViewportClient::getViewportSize() const
	{
		return math::float2(m_width, m_height);
	}

	void GameViewportClient::setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer)
	{
		m_renderTarget = viewportRenderTarget;
		m_depthTarget = depthRenderTarget;
		m_frameBuffer = frameBuffer;
	}

	GuGu::nvrhi::TextureHandle GameViewportClient::getRenderTarget() const
	{
		return m_renderTarget;
	}

	GuGu::nvrhi::TextureHandle GameViewportClient::getDepthTarget() const
	{
		return m_depthTarget;
	}

	GuGu::nvrhi::FramebufferHandle GameViewportClient::getFramebuffer() const
	{
		return m_frameBuffer;
	}

	GuGu::math::float2 GameViewportClient::getRenderTargetSize() const
	{
		return math::float2(m_renderTarget->getDesc().width, m_renderTarget->getDesc().height);
	}

	float GameViewportClient::getAspectRatio() const
	{
		return (float)m_width / m_height;
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

	GuGu::ViewportClient::ViewportState GameViewportClient::getViewportState() const
	{
		return ViewportClient::Runtime;
	}

	void GameViewportClient::setViewportState(ViewportState state)
	{
		
	}

	float GameViewportClient::getScreenScaleCompensation(math::float3 objWorldPos) const
	{
		return 0;
	}

	GuGu::ViewportClient::Gizmos GameViewportClient::getCurrentGizmosType() const
	{
		return Gizmos::Move;
	}

	GuGu::math::float4x4 GameViewportClient::getWorldToViewMatrix() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	GuGu::math::float4x4 GameViewportClient::getPespectiveMatrix() const
	{
		math::matrix perspectiveMatrix = math::perspProjD3DStyle(getFov(),
			getAspectRatio(), getNearPlane(), getFarPlane()
		);
		return perspectiveMatrix;
	}

	GuGu::math::float3 GameViewportClient::getCamPos() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	float GameViewportClient::getFov() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	std::shared_ptr<WindowWidget> CreateGameMainWindow()
	{
		
		std::shared_ptr<WindowWidget> mainWindow = std::make_shared<WindowWidget>();

		//1.先创建 window widget
		WIDGET_ASSIGN_NEW(WindowWidget, mainWindow)
		.Content
		(
			WIDGET_NEW(LevelViewport)
			.Content
			(
				NullWidget::getNullWidget()
			)
		)
		.ScreenPosition(math::float2(0.0f, 0.0f));

		AssetData assetData;
		assetData.m_filePath = "content/level1.json";
		assetData.m_fileName = "level1";
		assetData.m_assetType = typeof(Level);
		World::getWorld()->loadLevel(assetData);

		return mainWindow;
	}

}