#pragma once

#include "DeviceManager.h"

#include <Core/UI/UIData.h>
#include <Core/UI/BasicElement.h>
#include <Core/Model/GeometryHelper.h>
#include "TextureCache.h"

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
	class Level;
	class GStaticMesh;
	class StaticMeshComponent;
	class ViewportClient;
	class TerrainComponent;
	class WaterComponent;
	class CameraComponent;
	class TransformComponent;
	struct DrawItem
	{
		const MeshInfo* mesh;
		const BufferGroup* bufferGroup;//vertex buffer and index buffer
		const MeshGeometry* meshGeometry;
		nvrhi::BufferHandle m_worldMatrix;
		nvrhi::BufferHandle m_skinnedMatrix;
		bool m_isSkinned = false;
		nvrhi::BufferHandle m_pbrMaterial;
		nvrhi::TextureHandle m_albedoTexture;
		GStaticMesh* m_staticMesh;
	};
	//struct UIData;
	struct UIDrawInfo;
	class UIAtlas;
	class Demo : public IRenderPass
	{
	public:
		using IRenderPass::IRenderPass;

		bool Init(std::shared_ptr<UIData> uiData);

		virtual void Update(float fElapsedTimeSeconds) override;

		void BackBufferResizing() override;

		void Render() override;

		void LoadScene(const GuGuUtf8Str& filePath);

		nvrhi::TextureHandle getRenderTarget();

		void renderLevelRuntime(const std::shared_ptr<Level> inLevel, std::shared_ptr<ViewportClient> inViewportClient);

		void renderLevel(const std::shared_ptr<Level> inLevel, std::shared_ptr<ViewportClient> inViewportClient);

		void createVertexBufferAndIndexBuffer(GStaticMesh& staticMesh);

		void createDebugCameraFrustum(std::shared_ptr<CameraComponent> inCameraComponent, std::shared_ptr<TransformComponent> camTransform, std::shared_ptr<ViewportClient> inViewportClient);

		void createTerrainVertexBufferAndIndexBuffer(std::shared_ptr<TerrainComponent> terrainComponent);

		void createWaterVertexBufferAndIndexBuffer(std::shared_ptr<WaterComponent> waterComponent);

		void createUIVertexBufferAndIndexBuffer(std::shared_ptr<UIDrawInfo> inUIDrawInfo);

		void initRenderTargetAndDepthTarget(ViewportClient& viewportClient, math::float2 viewportSize);

		void updateAtlas(std::shared_ptr<UIAtlas> inAtlas);
	private:
		void RenderView(nvrhi::GraphicsState& graphicsState, nvrhi::GraphicsState& gridGraphicsState, math::float4x4 viewProjMatrix);

		nvrhi::BufferHandle createGeometryBuffer(const GuGuUtf8Str& debugName, const void* data, uint64_t dataSize, bool isVertexBuffer);	

		std::shared_ptr<RootFileSystem> m_rootFileSystem;

		nvrhi::CommandListHandle m_CommandList;
		TextureCache m_textureCache;

		std::shared_ptr<BufferGroup> m_buffers;
		std::shared_ptr<MeshInfo> m_meshInfo;
		std::shared_ptr<MeshInstance> m_meshInstance;
		nvrhi::BufferHandle m_cubeConstantBuffer;
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
			Color albedo;
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

		std::shared_ptr<UIData> m_uiData;

		void getNodeChildrens(GuGuUtf8Str, std::vector<GuGuUtf8Str>&);

		void selectionChanged(GuGuUtf8Str, SelectInfo::Type);

		std::shared_ptr<SceneGraphNode> m_currentSelectItems;

		GeometryHelper m_geometryHelper;
		std::shared_ptr<StaticMeshComponent> m_cylinderMeshComponent;

		//------gizmos------
		struct GizmosBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos;
		};

		struct GizmosPropertiesBuffer {
			dm::float3 color;
		};

		struct GizmosVertex
		{
			math::float3 position;
			math::float2 uv;
			math::float3 tangent;
			math::float3 normal;

			GizmosVertex(math::float3 inPosition, math::float2 inUV, math::float3 inTangent, math::float3 inNormal)
			{
				position = inPosition;
				uv = inUV;
				tangent = inTangent;
				normal = inNormal;
			}
		};

		nvrhi::ShaderHandle m_gizmosVertexShader;
		nvrhi::ShaderHandle m_gizmosPixelShader;
		nvrhi::BufferHandle m_gizmosConstantBuffer;
		std::vector<nvrhi::BufferHandle> m_gizmosPropertiesConstantBuffers;//3*3
		nvrhi::InputLayoutHandle m_gizmosInputLayout;
		nvrhi::BindingLayoutHandle m_gizmosBindingLayout;
		nvrhi::GraphicsPipelineHandle m_gizmosPipeline;

		struct GizmosConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos;
		};
		//------gizmos------

		//------terrain------
		nvrhi::ShaderHandle m_terrainVertexShader;
		nvrhi::ShaderHandle m_terrainPixelShader;
		nvrhi::InputLayoutHandle m_terrainInputLayout;
		nvrhi::BindingLayoutHandle m_terrainBindingLayout;
		nvrhi::BindingSetHandle m_terrainBindingSet;
		nvrhi::GraphicsPipelineHandle m_terrainPipeline;
		struct TerrainPropertiesBuffer {
			dm::float2 m_beginXZ;//x, z方向上的起始偏移
			dm::float2 m_xzOffset;//x, z方向上的位置偏移
			float m_heightScale;
		};
		struct TerrainConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos;
		};
		//------terrain------

		//------water------
		nvrhi::ShaderHandle m_waterVertexShader;
		nvrhi::ShaderHandle m_waterPixelShader;
		nvrhi::InputLayoutHandle m_waterInputLayout;
		nvrhi::BindingLayoutHandle m_waterBindingLayout;
		nvrhi::BindingSetHandle m_waterBindingSet;
		nvrhi::GraphicsPipelineHandle m_waterPipeline;
		struct WaterPropertiesBuffer {
			dm::float2 m_beginXZ;//x, z方向上的起始偏移
			dm::float2 m_xzOffset;//x, z方向上的位置偏移
			float m_heightScale;
		};
		struct WaterConstantBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos;
			float time;
		};
		//------water------

		//------debug draw camera------
		struct CameraBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos; //editor camera pos
		};

		struct CameraPropertiesBuffer {
			dm::float3 color;
		};

		struct CameraVertex
		{
			math::float3 position;

			CameraVertex(math::float3 inPosition)
			{
				position = inPosition;
			}
		};

		nvrhi::ShaderHandle m_cameraVertexShader;
		nvrhi::ShaderHandle m_cameraPixelShader;
		std::vector<nvrhi::BufferHandle> m_cameraConstantBuffer;
		std::vector<nvrhi::BufferHandle> m_cameraPropertiesConstantBuffers;
		nvrhi::InputLayoutHandle m_cameraInputLayout;
		nvrhi::BindingLayoutHandle m_cameraBindingLayout;
		nvrhi::GraphicsPipelineHandle m_cameraPipeline;
		//------debug draw camera------

		//------game ui------
		struct GameUIBufferEntry {
			dm::float4x4 viewProjMatrix;
			dm::float4x4 worldMatrix;
			dm::float3 camWorldPos;
		};

		struct GameUIPropertiesBuffer {
			dm::float3 color;
		};

		struct GameUIVertex
		{
			math::float2 m_uv;
			math::float3 m_position;
			math::float4 m_color;

			GameUIVertex(math::float3 inPosition, math::float2 inUV, math::float4 inColor)
			{
				m_position = inPosition;
				m_uv = inUV;
				m_color = inColor;
			}
		};

		nvrhi::ShaderHandle m_gameUIVertexShader;
		nvrhi::ShaderHandle m_gameUIPixelShader;
		std::vector<nvrhi::BufferHandle> m_gameUIConstantBuffer;
		std::vector<nvrhi::BufferHandle> m_gameUIPropertiesConstantBuffers;
		nvrhi::InputLayoutHandle m_gameUIInputLayout;
		nvrhi::BindingLayoutHandle m_gameUIBindingLayout;
		nvrhi::GraphicsPipelineHandle m_gameUIPipeline;
		//------game ui------

		uint32_t m_maxLightCounts = 4;
	};
}