#include <pch.h>

#include "D3D12Renderer.h"

#include <Application/Platform/Windows/WindowsApplication.h>
#include <Window/Platform/Windows/WindowsWindow.h>

namespace GuGu {
	D3D12Renderer::D3D12Renderer()
	{
		m_useWrapDevice = false;
		m_frameCount = 2;
		m_frameIndex = 0;
		m_rtvDescriptorSize = 0;
		m_aspectRatio = static_cast<float>(800) / static_cast<float>(600);//todo:fix this

		m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, 800.0f, 600.0f);
		m_scissorRect = CD3DX12_RECT(0.0f, 0.0f, 800.0f, 600.0f);
	}
	D3D12Renderer::~D3D12Renderer()
	{
	}
	void D3D12Renderer::init(std::shared_ptr<WindowWidget> inWindowWidget)
	{
		//------create device and factory------
		uint32_t dxgiFactoryFlags = 0;
#if defined(_DEBUG)
		
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			//enable additional debug layers
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
		
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
		
		if (m_useWrapDevice)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
			factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

			D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		}
		else
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
			getHardwareAdapter(factory.Get(), &hardwareAdapter);

			D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
		}
		

		//describe and create the command queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

		//describe and create the swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = m_frameCount;
		swapChainDesc.Width = 800;
		swapChainDesc.Height = 600;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		std::shared_ptr<WindowsApplication> windowsApplication = WindowsApplication::getApplication();
		std::vector<std::shared_ptr<WindowsWindow>> windows = windowsApplication->getPlatformWindows();

		//todo:fix this
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
		factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(), //swap chain needs the queue so that it can force a flush on it
			windows[0]->getNativeWindowHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain
		);

		swapChain.As(&m_swapChain);
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//create descriptor heaps
		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

			m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		//create frame resources
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

			//create a rtv for each frame
			for (uint32_t i = 0; i < m_frameCount; ++i)
			{
				m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
				m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, m_rtvDescriptorSize);
			}
		}

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

		loadAssets();
	}
	void D3D12Renderer::onRender()
	{
		populateCommandList();

		//execute the command list
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		//present the frame
		m_swapChain->Present(1, 0);

		WaitForPreviousFrame();
	}
	void D3D12Renderer::onDestroy()
	{
		WaitForPreviousFrame();

		CloseHandle(m_fenceEvent);
	}
	void D3D12Renderer::getHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
	{
		*ppAdapter = nullptr;

		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

		Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;

		if(SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (uint32_t adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex, requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE :
				DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter))); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}

				//check to see whether the adapter supports direct3d 12, but don't create the actual device yet
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (adapter.Get() == nullptr)
		{
			for (uint32_t adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}

				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		*ppAdapter = adapter.Detach();
	}
	void D3D12Renderer::loadAssets()
	{
		//create an empty root signature
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> signature;
			Microsoft::WRL::ComPtr<ID3DBlob> error;
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
		}

		//create the pipeline state, which includes compiling and loading shaders
		{
			Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
			Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
			//enable better shader debugging with the graphics debugging tools
			uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
			uint32_t compileFlags = 0;
#endif
			D3DCompileFromFile(L"..//..//asset//shaders.hlsl", nullptr, &m_include, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
			D3DCompileFromFile(L"..//..//asset//shaders.hlsl", nullptr, &m_include, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

			//define the vertex input layout
			D3D12_INPUT_ELEMENT_DESC inputElementsDescs[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};

			//describe and create the graphics pipeline state object (PSO)
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.InputLayout = { inputElementsDescs, _countof(inputElementsDescs) };
			psoDesc.pRootSignature = m_rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
		}

		//create the command list
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

		m_commandList->Close();

		//create the vertex buffer
		{
			//define the geometry for a triangle
			Vertex triangleVertices[] = {
				{{0.0f, 0.25f * m_aspectRatio, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
				{{0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
				{{-0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
			};

			const uint32_t vertexBufferSize = sizeof(triangleVertices);

			m_device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_vertexBuffer));

			//copy the triangle data to the vertex buffer
			uint8_t* pVertexDataBegin;
			CD3DX12_RANGE readRange(0, 0);
			m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
			memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
			m_vertexBuffer->Unmap(0, nullptr);

			//initialize the vertex buffer view
			m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = sizeof(Vertex);
			m_vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		//create synchronization objects
		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		m_fenceValue = 1;

		//create an event handle to use for frame synchronization
		m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

		WaitForPreviousFrame();
	}
	void D3D12Renderer::populateCommandList()
	{
		m_commandAllocator->Reset();

		m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

		//set necessary state
		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);

		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
		m_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

		//record commands
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_commandList->DrawInstanced(3, 1, 0, 0);

		m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

		m_commandList->Close();
	}
	void D3D12Renderer::WaitForPreviousFrame()
	{
		const uint64_t fence = m_fenceValue;
		m_commandQueue->Signal(m_fence.Get(), fence);
		++m_fenceValue;

		//wait unit the previous frame is finished
		if (m_fence->GetCompletedValue() < fence)
		{
			m_fence->SetEventOnCompletion(fence, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	}
	//std::shared_ptr<Renderer> CreateRendererFactory()
	//{
	//	return std::make_shared<D3D12Renderer>();
	//}
}