#pragma once
#include "Renderer.h"

#include "CustomInclude.h"

namespace GuGu {
	class WindowWidget;
	class D3D12Renderer : public Renderer
	{
	public:
		D3D12Renderer();

		virtual ~D3D12Renderer();

		virtual void init(std::shared_ptr<WindowWidget> inWindowWidget) override;

		virtual void onRender() override;

		virtual void onDestroy() override;

		void getHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);

		struct Vertex
		{
			float position[3];
			float color[4]; //todo:fix this
		};
	private:
		void loadAssets();

		void populateCommandList();

		void WaitForPreviousFrame();

		bool m_useWrapDevice;

		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT	m_scissorRect;

		Microsoft::WRL::ComPtr<ID3D12Device> m_device;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[2];//2 is frame count

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		HANDLE m_fenceEvent;

		uint32_t m_frameCount;
		uint32_t m_frameIndex;
		uint32_t m_rtvDescriptorSize;
		uint64_t m_fenceValue;

		float m_aspectRatio;

		CustomInclude m_include;
	};
}