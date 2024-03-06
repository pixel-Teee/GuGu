#pragma once

#include "DeviceManager.h"

namespace GuGu {
	class RootFileSystem;
	class BufferGroup;
	class MeshInfo;
	class MeshInstance;
	class SceneGraph;
	class MeshGeometry;
	struct DrawItem
	{
		const MeshInfo* mesh;
		const BufferGroup* bufferGroup;//vertex buffer and index buffer
		const MeshGeometry* meshGeometry;
		nvrhi::BufferHandle m_worldMatrix;
		nvrhi::BufferHandle m_skinnedMatrix;
		bool m_isSkinned = false;
	};

	class Demo : public IRenderPass
	{
	public:
		using IRenderPass::IRenderPass;

		bool Init();

		virtual void Update(float fElapsedTimeSeconds) override;

		void BackBufferResizing() override;

		void Render(nvrhi::IFramebuffer* framebuffer) override;

		void LoadScene(const GuGuUtf8Str& filePath);
	private:
		void RenderView(nvrhi::GraphicsState& graphicsState, math::float4x4 viewProjMatrix);

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
		nvrhi::InputLayoutHandle m_InputLayout;
		nvrhi::BindingLayoutHandle m_BindingLayout;
		nvrhi::BindingLayoutHandle m_SkinnedBindingLayout;
		nvrhi::BindingSetHandle m_BindingSet;
		nvrhi::BindingSetHandle m_SkinnedBindingSet;
		nvrhi::GraphicsPipelineHandle m_Pipeline;
		nvrhi::GraphicsPipelineHandle m_SkinnedPipeline;
		nvrhi::TextureHandle m_Texture;

		struct ConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			float padding[16 * 3];
		};

		std::vector<DrawItem> m_drawItems;

		float m_wallClockTime = 0.0f;
	};
}