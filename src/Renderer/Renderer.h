#pragma once

namespace GuGu {
	class DeviceManager;
	class VertexBuffer;
	class UIRenderPass;
	class Demo;
	class Renderer {
	public:
		Renderer();

		virtual ~Renderer();

		virtual void init();

		virtual void onRender();

		virtual void onDestroy();

        virtual void onResize(int32_t width, int32_t height);

		UIRenderPass* getUIRenderPass() const;
	protected:
		DeviceManager* m_deviceManager;//todo:remove this	

		VertexBuffer* m_vertexBuffer;

		UIRenderPass* m_UIRenderPass;

		Demo* m_demo;
	};

	//static std::shared_ptr<Renderer> CreateRendererFactory();
}