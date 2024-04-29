#pragma once

#include "DeviceManager.h"

#include <Core/UI/UIData.h>
#include <Core/UI/BasicElement.h>

namespace GuGu {
	class BindingCache;
	class RootFileSystem;
	class BufferGroup;
	class MeshInfo;
	class MeshInstance;
	class SceneGraph;
	class MeshGeometry;
	class CommonRenderPasses;
	class SceneGraphNode;
	struct DrawItem
	{
		const MeshInfo* mesh;
		const BufferGroup* bufferGroup;//vertex buffer and index buffer
		const MeshGeometry* meshGeometry;
		nvrhi::BufferHandle m_worldMatrix;
		nvrhi::BufferHandle m_skinnedMatrix;
		bool m_isSkinned = false;
		nvrhi::BufferHandle m_pbrMaterial;
	};
	//struct UIData;
	class Demo : public IRenderPass
	{
	public:
		using IRenderPass::IRenderPass;

		bool Init(std::shared_ptr<UIData> uiData);

		virtual void Update(float fElapsedTimeSeconds) override;

		void BackBufferResizing() override;

		void Render(nvrhi::IFramebuffer* framebuffer) override;

		void LoadScene(const GuGuUtf8Str& filePath);

		nvrhi::TextureHandle getRenderTarget();
	private:
		void RenderView(nvrhi::GraphicsState& graphicsState, nvrhi::GraphicsState& gridGraphicsState, math::float4x4 viewProjMatrix);

		nvrhi::BufferHandle createGeometryBuffer(const GuGuUtf8Str& debugName, const void* data, uint64_t dataSize, bool isVertexBuffer);	

		std::shared_ptr<RootFileSystem> m_rootFileSystem;

		nvrhi::CommandListHandle m_CommandList;

		std::shared_ptr<BufferGroup> m_buffers;
		std::shared_ptr<MeshInfo> m_meshInfo;
		std::shared_ptr<MeshInstance> m_meshInstance;
		std::shared_ptr<SceneGraph> m_sceneGraph;

		std::shared_ptr<SceneGraph> m_testGraph;

		nvrhi::SamplerHandle m_pointWrapSampler;
		nvrhi::ShaderHandle m_VertexShader;
		nvrhi::ShaderHandle m_PixelShader;
		nvrhi::ShaderHandle m_SkinnedVertexShader;
		std::vector<nvrhi::BufferHandle> m_ConstantBuffers;
		std::vector<nvrhi::BufferHandle> m_PbrMaterialBuffers;
		nvrhi::BufferHandle m_LightBuffers;
		nvrhi::BufferHandle m_PassBuffers;
		nvrhi::InputLayoutHandle m_InputLayout;
		nvrhi::BindingLayoutHandle m_BindingLayout;
		nvrhi::BindingLayoutHandle m_SkinnedBindingLayout;
		nvrhi::BindingSetHandle m_BindingSet;
		nvrhi::BindingSetHandle m_SkinnedBindingSet;
		nvrhi::GraphicsPipelineHandle m_Pipeline;
		nvrhi::GraphicsPipelineHandle m_SkinnedPipeline;
		nvrhi::TextureHandle m_Texture;

		//------grid------
		struct GridVertex
		{
			math::float3 position;
			math::float2 uv;

			GridVertex(math::float3 inPosition, math::float2 inUV)
			{
				position = inPosition;
				uv = inUV;
			}
		};

		nvrhi::ShaderHandle m_gridVertexShader;
		nvrhi::ShaderHandle m_gridPixelShader;
		nvrhi::BufferHandle m_gridConstantBuffer;
		nvrhi::BufferHandle m_gridPropertiesConstantBuffer;
		nvrhi::InputLayoutHandle m_gridInputLayout;
		nvrhi::BindingLayoutHandle m_gridBindingLayout;
		nvrhi::BindingSetHandle m_gridBindingSet;
		nvrhi::GraphicsPipelineHandle m_gridPipeline;
		nvrhi::BufferHandle m_gridVertexBuffer;
		std::vector<GridVertex> m_gridVertices;

		struct GridConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos;
		};
		//------grid------

		struct ConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			float padding[32];
		};

		struct PbrMaterial {
			math::float3 albedo;
			float metallic;
			float roughness;
			float ao;
		};

		struct Pass {
			math::float3 camPos;
		};

		struct Light
		{
			math::float4 lightPositions[4];
			math::float4 lightColors[4];
		};

		std::vector<DrawItem> m_drawItems;

		float m_wallClockTime = 0.0f;

		std::shared_ptr<CommonRenderPasses> m_commonRenderPass;
		std::shared_ptr<BindingCache> m_bindingCache;

		math::uint2 m_renderTargetSize;
		nvrhi::TextureHandle m_renderTarget;
		nvrhi::TextureHandle m_depthTarget;
		nvrhi::FramebufferHandle m_frameBuffer;

		void initRenderTargetAndDepthTarget();

		std::shared_ptr<UIData> m_uiData;

		void getNodeChildrens(GuGuUtf8Str, std::vector<GuGuUtf8Str>&);

		void selectionChanged(GuGuUtf8Str, SelectInfo::Type);

		std::shared_ptr<SceneGraphNode> m_currentSelectItems;
	};
}