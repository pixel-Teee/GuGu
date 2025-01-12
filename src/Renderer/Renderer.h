#pragma once

namespace GuGu {
	class DeviceManager;
	class VertexBuffer;
	class UIRenderPass;
	class Demo;
	class WindowWidget;
	class Window;
	class Renderer {
	public:
		Renderer();

		virtual ~Renderer();

		virtual void init(std::shared_ptr<WindowWidget> inWindowWidget = nullptr);

		virtual void onRender();

		virtual void onDestroy();

        virtual void onResize(int32_t width, int32_t height);

		virtual void requestResize(std::shared_ptr<Window> inWindow);

		UIRenderPass* getUIRenderPass() const;

		Demo* getDemoPass() const;

		VertexBuffer* getVertexBufferPass() const;

		DeviceManager* getDeviceManager() const;

		virtual void createSurface(std::shared_ptr<WindowWidget> windowWidget);
		virtual void createSwapChain(std::shared_ptr<WindowWidget> windowWidget);
		virtual void destoryOldSwapChain(std::shared_ptr<WindowWidget> windowWidget);
		virtual void resizeSwapChain(std::shared_ptr<WindowWidget> windowWidget);

		void onWindowDestroyed(const std::shared_ptr<WindowWidget>& inWindow);
	protected:
		DeviceManager* m_deviceManager;//todo:remove this	

		VertexBuffer* m_vertexBuffer;

		UIRenderPass* m_UIRenderPass;

		Demo* m_demo;
	};

	//static std::shared_ptr<Renderer> CreateRendererFactory();
}