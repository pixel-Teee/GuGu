#include <pch.h>

#include "Renderer.h"

namespace GuGu {
	Renderer::Renderer()
	{
	}
	Renderer::~Renderer()
	{
	}
	void Renderer::init()
	{
		//nothing
	}
	void Renderer::onRender()
	{
	}
	void Renderer::onDestroy()
	{
	}
    void Renderer::onResize(int32_t width, int32_t height)
	{
    }
	UIRenderPass* Renderer::getUIRenderPass() const
	{
		return m_UIRenderPass;
	}
	Demo* Renderer::getDemoPass() const
	{
		return m_demo;
	}
}