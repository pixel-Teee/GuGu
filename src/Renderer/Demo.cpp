#include <pch.h>

#include "Demo.h"

#include <Application/Application.h>

#include <Core/FileSystem/FileSystem.h>

#include <Core/SceneGraph/SceneTypes.h>
#include <Core/SceneGraph/SceneGraph.h>
#include <Core/Timer.h>

#include <Renderer/TextureCache.h>

#include "CubeGeometry.h"
#include "BindingCache.h"
#include "ShaderFactory.h"
#include "CommonRenderPasses.h"
#include "utils.h"

#include <gltf/gltfLoader.h>

#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/GamePlay/StaticMeshComponent.h>
#include <Core/GamePlay/LightComponent.h>
#include <Core/GamePlay/MaterialComponent.h>
#include <Core/GamePlay/CameraComponent.h>
#include <Core/GamePlay/TerrainComponent.h>
#include <Core/GamePlay/WaterComponent.h>
#include <Core/GamePlay/RenderComponent/CubeComponent.h>
#include <Core/Model/StaticMesh.h>
#include <Core/GamePlay/ViewportClient.h>
#include <Core/Texture/GTexture.h>

#include <Core/GamePlay/GameUI/UITransformComponent.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Core/GamePlay/GameUI/UIDrawInfo.h>
#include <Core/GamePlay/GameUI/TextComponent.h>
#include <Core/GamePlay/GameUI/GFont.h>

#include <Core/AssetManager/AssetManager.h>

namespace GuGu {
	inline void AppendBufferRange(nvrhi::BufferRange& range, size_t size, uint64_t& currentBufferSize)
	{
		range.byteOffset = currentBufferSize;
		range.byteSize = size;
		currentBufferSize += range.byteSize;
	}

	void Demo::updateAtlas(std::shared_ptr<UIAtlas> inAtlas)
	{
		//check texture handle
		//if (inAtlas->m_texture->m_texture == nullptr)
		//{
		//
		//}

		nvrhi::TextureDesc textureDesc;
		textureDesc.format = (nvrhi::Format)inAtlas->m_texture->m_format;
		textureDesc.width = inAtlas->m_texture->m_width;
		textureDesc.height = inAtlas->m_texture->m_height;
		textureDesc.depth = 1;
		textureDesc.arraySize = 1;
		textureDesc.dimension = nvrhi::TextureDimension::Texture2D;//todo:fix this
		textureDesc.mipLevels = 1;
		textureDesc.debugName = "runtime-ui-atlas";
		textureDesc.isRenderTarget = true;

		//create texture
		inAtlas->m_texture->m_texture = GetDevice()->createTexture(textureDesc);

		const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(inAtlas->m_fontAtlasData.data()));
		//write to texture
		m_CommandList->beginTrackingTextureState(inAtlas->m_texture->m_texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
		m_CommandList->writeTexture(inAtlas->m_texture->m_texture, 0, 0, dataPointer, inAtlas->m_texture->m_width, 1);
		m_CommandList->setPermanentTextureState(inAtlas->m_texture->m_texture, nvrhi::ResourceStates::ShaderResource);//todo:fix this
		m_CommandList->commitBarriers();

		inAtlas->m_bNeedToUpdateAtlas = false;
	}

	void Demo::createVertexBufferAndIndexBuffer(GStaticMesh& staticMesh)
	{
		std::vector<uint32_t>& indexData = staticMesh.m_indexData;
		std::vector<math::float3>& positionData = staticMesh.m_positionData;
		std::vector<math::float2>& texCoord1Data = staticMesh.m_texCoord1Data;
		std::vector<math::float2>& texCoord2Data = staticMesh.m_texCoord2Data;
		std::vector<math::float3>& normalData = staticMesh.m_normalData;
		std::vector<math::float3>& tangentData = staticMesh.m_tangentData;
		Array<math::vector<int16_t, 4>>& jointData = staticMesh.m_jointData;//指向骨骼矩阵的索引
		std::vector<math::float4>& weightData = staticMesh.m_weightData;
		std::array<nvrhi::BufferRange, size_t(GVertexAttribute::Count)>& vertexBufferRanges = staticMesh.m_vertexBufferRanges;

		if (!indexData.empty() && !staticMesh.m_indexBuffer)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = indexData.size() * sizeof(uint32_t);
			bufferDesc.debugName = "IndexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.format = nvrhi::Format::R32_UINT;
			//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

			staticMesh.m_indexBuffer = GetDevice()->createBuffer(bufferDesc);

			m_CommandList->beginTrackingBufferState(staticMesh.m_indexBuffer, nvrhi::ResourceStates::Common);

			m_CommandList->writeBuffer(staticMesh.m_indexBuffer, indexData.data(), indexData.size() * sizeof(uint32_t));

			nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;

			m_CommandList->setPermanentBufferState(staticMesh.m_indexBuffer, state);
			m_CommandList->commitBarriers();
		}

		if (!staticMesh.m_vertexBuffer)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isVertexBuffer = true;
			bufferDesc.byteSize = 0;
			bufferDesc.debugName = "VertexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;

			if (!positionData.empty())
			{
				AppendBufferRange(staticMesh.getVertexBufferRange(GVertexAttribute::Position),
					positionData.size() * sizeof(positionData[0]), bufferDesc.byteSize);
			}

			if (!normalData.empty())
			{
				AppendBufferRange(staticMesh.getVertexBufferRange(GVertexAttribute::Normal),
					normalData.size() * sizeof(normalData[0]), bufferDesc.byteSize);
			}

			if (!tangentData.empty())
			{
				AppendBufferRange(staticMesh.getVertexBufferRange(GVertexAttribute::Tangent),
					tangentData.size() * sizeof(tangentData[0]), bufferDesc.byteSize);
			}

			if (!texCoord1Data.empty())
			{
				AppendBufferRange(staticMesh.getVertexBufferRange(GVertexAttribute::TexCoord1),
					texCoord1Data.size() * sizeof(texCoord1Data[0]), bufferDesc.byteSize);
			}

			if (!weightData.empty())
			{
				AppendBufferRange(staticMesh.getVertexBufferRange(GVertexAttribute::JointWeights),
					weightData.size() * sizeof(weightData[0]), bufferDesc.byteSize);
			}

			if (!jointData.empty())
			{
				AppendBufferRange(staticMesh.getVertexBufferRange(GVertexAttribute::JointIndices),
					jointData.size() * sizeof(jointData[0]), bufferDesc.byteSize);
			}

			staticMesh.m_vertexBuffer = GetDevice()->createBuffer(bufferDesc);

			m_CommandList->beginTrackingBufferState(staticMesh.m_vertexBuffer, nvrhi::ResourceStates::Common);

			if (!positionData.empty())
			{
				const auto& range = staticMesh.getVertexBufferRange(GVertexAttribute::Position);
				m_CommandList->writeBuffer(staticMesh.m_vertexBuffer, positionData.data(), range.byteSize, range.byteOffset);
			}

			if (!normalData.empty())
			{
				const auto& range = staticMesh.getVertexBufferRange(GVertexAttribute::Normal);
				m_CommandList->writeBuffer(staticMesh.m_vertexBuffer, normalData.data(), range.byteSize, range.byteOffset);
			}

			if (!tangentData.empty())
			{
				const auto& range = staticMesh.getVertexBufferRange(GVertexAttribute::Tangent);
				m_CommandList->writeBuffer(staticMesh.m_vertexBuffer, tangentData.data(), range.byteSize, range.byteOffset);
			}

			if (!texCoord1Data.empty())
			{
				const auto& range = staticMesh.getVertexBufferRange(GVertexAttribute::TexCoord1);
				m_CommandList->writeBuffer(staticMesh.m_vertexBuffer, texCoord1Data.data(), range.byteSize, range.byteOffset);
			}

			if (!weightData.empty())
			{
				const auto& range = staticMesh.getVertexBufferRange(GVertexAttribute::JointWeights);
				m_CommandList->writeBuffer(staticMesh.m_vertexBuffer, weightData.data(), range.byteSize, range.byteOffset);
			}

			if (!jointData.empty())
			{
				const auto& range = staticMesh.getVertexBufferRange(GVertexAttribute::JointIndices);
				m_CommandList->writeBuffer(staticMesh.m_vertexBuffer, jointData.data(), range.byteSize, range.byteOffset);
			}

			nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer | nvrhi::ResourceStates::ShaderResource;

			m_CommandList->setPermanentBufferState(staticMesh.m_vertexBuffer, state);
			m_CommandList->commitBarriers();
		}
	}

	void Demo::createDebugCameraFrustum(std::shared_ptr<CameraComponent> inCameraComponent, std::shared_ptr<TransformComponent> camTransform, std::shared_ptr<ViewportClient> inViewportClient)
	{
		math::float3 frustumPoints[] = {
			{-1, 1, 0}, //near plane
			{1, 1, 0}, //near plane
			{1, -1, 0}, //near plane
			{-1, -1, 0}, //near plane
			{-1, 1, 1}, //far plane
			{1, 1, 1}, //far plane
			{1, -1, 1}, //far plane
			{-1, -1, 1}, //far plane
		};
		
		uint32_t frustumPointsIndex[] =
		{ 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 };

		math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(inCameraComponent->getFov()),
			inViewportClient->getAspectRatio(), inCameraComponent->getNearPlane(), inCameraComponent->getFarPlane()
		);
		math::float4x4 viewProjMatrix = math::inverse(math::affineToHomogeneous(camTransform->GetLocalToWorldTransformFloat())) * projMatrix;
		math::float4x4 invViewProjMatrix = math::inverse(viewProjMatrix);
		std::vector<math::float3> newFrustumPoints;
		newFrustumPoints.resize(8);
		for (size_t i = 0; i < 8; ++i)
		{
			//new point
			math::float4 tempPoint = math::float4(frustumPoints[i].x, frustumPoints[i].y, frustumPoints[i].z, 1.0f) * invViewProjMatrix;
			newFrustumPoints[i].x = tempPoint.x  / tempPoint.w;
			newFrustumPoints[i].y = tempPoint.y  / tempPoint.w;
			newFrustumPoints[i].z = tempPoint.z  / tempPoint.w;
			//newFrustumPoints[i].w = tempPoint.w  / tempPoint.w;
		}

		if (!inCameraComponent->m_debugCameraFrustumIndexBuffer)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = 24 * sizeof(uint32_t);
			bufferDesc.debugName = "CameraIndexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.format = nvrhi::Format::R32_UINT;
			//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

			inCameraComponent->m_debugCameraFrustumIndexBuffer = GetDevice()->createBuffer(bufferDesc);
			m_CommandList->beginTrackingBufferState(inCameraComponent->m_debugCameraFrustumIndexBuffer, nvrhi::ResourceStates::Common);
			m_CommandList->writeBuffer(inCameraComponent->m_debugCameraFrustumIndexBuffer, frustumPointsIndex, 24 * sizeof(uint32_t));
			nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;
			m_CommandList->setPermanentBufferState(inCameraComponent->m_debugCameraFrustumIndexBuffer, state);
			m_CommandList->commitBarriers();
		}


		if (!inCameraComponent->m_debugCameraFrustumVertexBuffer)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isVertexBuffer = true;
			bufferDesc.byteSize = 24 * sizeof(math::float3);
			bufferDesc.debugName = "CameraVertexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.keepInitialState = true;
			bufferDesc.canHaveRawViews = true;
			inCameraComponent->m_debugCameraFrustumVertexBuffer = GetDevice()->createBuffer(bufferDesc);
		}

		//m_CommandList->beginTrackingBufferState(inCameraComponent->m_debugCameraFrustumVertexBuffer, nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(inCameraComponent->m_debugCameraFrustumVertexBuffer, newFrustumPoints.data(), sizeof(frustumPoints), 0);
		nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer;
		//m_CommandList->setPermanentBufferState(inCameraComponent->m_debugCameraFrustumVertexBuffer, state);
		m_CommandList->commitBarriers();
	}

	void Demo::createTerrainVertexBufferAndIndexBuffer(std::shared_ptr<TerrainComponent> terrainComponent)
	{
		std::vector<uint32_t>& indexData = terrainComponent->m_indexData;
		std::vector<math::float3>& positionData = terrainComponent->m_vertexData;

		if (!indexData.empty() && !terrainComponent->m_indexBufferHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = indexData.size() * sizeof(uint32_t);
			bufferDesc.debugName = "IndexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.format = nvrhi::Format::R32_UINT;
			//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

			terrainComponent->m_indexBufferHandle = GetDevice()->createBuffer(bufferDesc);

			m_CommandList->beginTrackingBufferState(terrainComponent->m_indexBufferHandle, nvrhi::ResourceStates::Common);

			m_CommandList->writeBuffer(terrainComponent->m_indexBufferHandle, indexData.data(), indexData.size() * sizeof(uint32_t));

			nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;

			m_CommandList->setPermanentBufferState(terrainComponent->m_indexBufferHandle, state);
			m_CommandList->commitBarriers();
		}

		if (!terrainComponent->m_vertexBufferHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isVertexBuffer = true;
			bufferDesc.byteSize = 0;
			bufferDesc.debugName = "VertexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;

			if (!positionData.empty())
			{
				nvrhi::BufferRange bufferRange;
				bufferRange.byteSize = 0;
				bufferRange.byteOffset = 0;
				AppendBufferRange(bufferRange,
					positionData.size() * sizeof(positionData[0]), bufferDesc.byteSize);
			}

			terrainComponent->m_vertexBufferHandle = GetDevice()->createBuffer(bufferDesc);

			m_CommandList->beginTrackingBufferState(terrainComponent->m_vertexBufferHandle, nvrhi::ResourceStates::Common);

			if (!positionData.empty())
			{
				nvrhi::BufferRange bufferRange;
				bufferRange.byteSize = positionData.size() * sizeof(positionData[0]);
				bufferRange.byteOffset = 0;
				m_CommandList->writeBuffer(terrainComponent->m_vertexBufferHandle, positionData.data(), bufferRange.byteSize, bufferRange.byteOffset);
			}

			nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer | nvrhi::ResourceStates::ShaderResource;

			m_CommandList->setPermanentBufferState(terrainComponent->m_vertexBufferHandle, state);
			m_CommandList->commitBarriers();
		}
	}

	void Demo::createWaterVertexBufferAndIndexBuffer(std::shared_ptr<WaterComponent> waterComponent)
	{
		std::vector<uint32_t>& indexData = waterComponent->m_indexData;
		std::vector<math::float3>& positionData = waterComponent->m_vertexData;

		if (!indexData.empty() && !waterComponent->m_indexBufferHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = indexData.size() * sizeof(uint32_t);
			bufferDesc.debugName = "IndexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.format = nvrhi::Format::R32_UINT;
			//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

			waterComponent->m_indexBufferHandle = GetDevice()->createBuffer(bufferDesc);

			m_CommandList->beginTrackingBufferState(waterComponent->m_indexBufferHandle, nvrhi::ResourceStates::Common);

			m_CommandList->writeBuffer(waterComponent->m_indexBufferHandle, indexData.data(), indexData.size() * sizeof(uint32_t));

			nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;

			m_CommandList->setPermanentBufferState(waterComponent->m_indexBufferHandle, state);
			m_CommandList->commitBarriers();
		}

		if (!waterComponent->m_vertexBufferHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isVertexBuffer = true;
			bufferDesc.byteSize = 0;
			bufferDesc.debugName = "VertexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;

			if (!positionData.empty())
			{
				nvrhi::BufferRange bufferRange;
				bufferRange.byteSize = 0;
				bufferRange.byteOffset = 0;
				AppendBufferRange(bufferRange,
					positionData.size() * sizeof(positionData[0]), bufferDesc.byteSize);
			}

			waterComponent->m_vertexBufferHandle = GetDevice()->createBuffer(bufferDesc);

			m_CommandList->beginTrackingBufferState(waterComponent->m_vertexBufferHandle, nvrhi::ResourceStates::Common);

			if (!positionData.empty())
			{
				nvrhi::BufferRange bufferRange;
				bufferRange.byteSize = positionData.size() * sizeof(positionData[0]);
				bufferRange.byteOffset = 0;
				m_CommandList->writeBuffer(waterComponent->m_vertexBufferHandle, positionData.data(), bufferRange.byteSize, bufferRange.byteOffset);
			}

			nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer | nvrhi::ResourceStates::ShaderResource;

			m_CommandList->setPermanentBufferState(waterComponent->m_vertexBufferHandle, state);
			m_CommandList->commitBarriers();
		}
	}

	void Demo::createUIVertexBufferAndIndexBuffer(std::shared_ptr<UIDrawInfo> inUIDrawInfo)
	{
		//draw info
		
		if (!inUIDrawInfo->m_uiIndexHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = inUIDrawInfo->m_uiIndices.size() * sizeof(uint32_t);
			bufferDesc.debugName = "GameUIIndexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.format = nvrhi::Format::R32_UINT;
			//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

			inUIDrawInfo->m_uiIndexHandle = GetDevice()->createBuffer(bufferDesc);
			m_CommandList->beginTrackingBufferState(inUIDrawInfo->m_uiIndexHandle, nvrhi::ResourceStates::Common);
			m_CommandList->writeBuffer(inUIDrawInfo->m_uiIndexHandle, inUIDrawInfo->m_uiIndices.data(), inUIDrawInfo->m_uiIndices.size() * sizeof(uint32_t));
			nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;
			m_CommandList->setPermanentBufferState(inUIDrawInfo->m_uiIndexHandle, state);
			m_CommandList->commitBarriers();
		}


		if (!inUIDrawInfo->m_uiVertexHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isVertexBuffer = true;
			bufferDesc.byteSize = inUIDrawInfo->m_uiVertex.size() * sizeof(GameUIVertex);
			bufferDesc.debugName = "GameUIVertexHandle";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.keepInitialState = true;
			bufferDesc.canHaveRawViews = true;
			inUIDrawInfo->m_uiVertexHandle = GetDevice()->createBuffer(bufferDesc);
		}

		//m_CommandList->beginTrackingBufferState(inCameraComponent->m_debugCameraFrustumVertexBuffer, nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(inUIDrawInfo->m_uiVertexHandle, inUIDrawInfo->m_uiVertex.data(), inUIDrawInfo->m_uiVertex.size() * sizeof(GameUIVertex), 0);
		nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer;
		//m_CommandList->setPermanentBufferState(inCameraComponent->m_debugCameraFrustumVertexBuffer, state);
		m_CommandList->commitBarriers();
	}

	void Demo::createUIDebugVertexBufferAndIndexBuffer(std::shared_ptr<UIDebugInfo> inUIDebugInfo)
	{
		//draw info

		if (!inUIDebugInfo->m_uiDebugIndexHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isIndexBuffer = true;
			bufferDesc.byteSize = inUIDebugInfo->m_uiDebugIndices.size() * sizeof(uint32_t);
			bufferDesc.debugName = "GameUIDebugIndexBuffer";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.format = nvrhi::Format::R32_UINT;
			//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

			inUIDebugInfo->m_uiDebugIndexHandle = GetDevice()->createBuffer(bufferDesc);
			m_CommandList->beginTrackingBufferState(inUIDebugInfo->m_uiDebugIndexHandle, nvrhi::ResourceStates::Common);
			m_CommandList->writeBuffer(inUIDebugInfo->m_uiDebugIndexHandle, inUIDebugInfo->m_uiDebugIndices.data(), inUIDebugInfo->m_uiDebugIndices.size() * sizeof(uint32_t));
			nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;
			m_CommandList->setPermanentBufferState(inUIDebugInfo->m_uiDebugIndexHandle, state);
			m_CommandList->commitBarriers();
		}


		if (!inUIDebugInfo->m_uiDebugVertexHandle)
		{
			nvrhi::BufferDesc bufferDesc;
			bufferDesc.isVertexBuffer = true;
			bufferDesc.byteSize = inUIDebugInfo->m_uiDebugVertex.size() * sizeof(math::float3);
			bufferDesc.debugName = "GameUIDebugVertexHandle";
			bufferDesc.canHaveTypedViews = true;
			bufferDesc.canHaveRawViews = true;
			bufferDesc.keepInitialState = true;
			bufferDesc.canHaveRawViews = true;
			inUIDebugInfo->m_uiDebugVertexHandle = GetDevice()->createBuffer(bufferDesc);
		}

		//m_CommandList->beginTrackingBufferState(inCameraComponent->m_debugCameraFrustumVertexBuffer, nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(inUIDebugInfo->m_uiDebugVertexHandle, inUIDebugInfo->m_uiDebugVertex.data(), inUIDebugInfo->m_uiDebugVertex.size() * sizeof(math::float3), 0);
		nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer;
		//m_CommandList->setPermanentBufferState(inCameraComponent->m_debugCameraFrustumVertexBuffer, state);
		m_CommandList->commitBarriers();
	}

	bool Demo::Init(std::shared_ptr<UIData> uiData)
	{
		m_uiData = uiData;

		GuGuUtf8Str assetPath = Application::GetDirectoryWithExecutable();
#if 1
		std::shared_ptr<NativeFileSystem> nativeFileSystem = std::make_shared<NativeFileSystem>(assetPath);
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("asset", nativeFileSystem);
#else
		std::shared_ptr<ArchiverFileSystem> archiverFileSystem;
		if (assetPath == "")
			archiverFileSystem = std::make_shared<ArchiverFileSystem>("archiver.bin");//this is root file
		else
			archiverFileSystem = std::make_shared<ArchiverFileSystem>(assetPath + "/archiver.bin");
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("asset", archiverFileSystem);
#endif	
		m_renderTargetSize = math::uint2(0, 0);

		m_CommandList = GetDevice()->createCommandList();
		m_CommandList->open();

		m_buffers = std::make_shared<BufferGroup>();
		m_buffers->indexBuffer = createGeometryBuffer("IndexBuffer", g_Indices, sizeof(g_Indices), false);

		//non-interleaved
		uint64_t vertexBufferSize = 0;
		m_buffers->getVertexBufferRange(VertexAttribute::Position).setByteOffset(vertexBufferSize).setByteSize(sizeof(g_Positions)); vertexBufferSize += sizeof(g_Positions);
		m_buffers->getVertexBufferRange(VertexAttribute::TexCoord1).setByteOffset(vertexBufferSize).setByteSize(sizeof(g_TexCoords)); vertexBufferSize += sizeof(g_TexCoords);
		m_buffers->getVertexBufferRange(VertexAttribute::Normal).setByteOffset(vertexBufferSize).setByteSize(sizeof(g_Normals)); vertexBufferSize += sizeof(g_Normals);
		m_buffers->getVertexBufferRange(VertexAttribute::Tangent).setByteOffset(vertexBufferSize).setByteSize(sizeof(g_Tangents)); vertexBufferSize += sizeof(g_Tangents);
		m_buffers->vertexBuffer = createGeometryBuffer("VertexBuffer", nullptr, vertexBufferSize, true);

		m_CommandList->beginTrackingBufferState(m_buffers->vertexBuffer, nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(m_buffers->vertexBuffer, g_Positions, sizeof(g_Positions), m_buffers->getVertexBufferRange(VertexAttribute::Position).byteOffset);
		m_CommandList->writeBuffer(m_buffers->vertexBuffer, g_TexCoords, sizeof(g_TexCoords), m_buffers->getVertexBufferRange(VertexAttribute::TexCoord1).byteOffset);
		m_CommandList->writeBuffer(m_buffers->vertexBuffer, g_Normals, sizeof(g_Normals), m_buffers->getVertexBufferRange(VertexAttribute::Normal).byteOffset);
		m_CommandList->writeBuffer(m_buffers->vertexBuffer, g_Tangents, sizeof(g_Tangents), m_buffers->getVertexBufferRange(VertexAttribute::Tangent).byteOffset);
		m_CommandList->setPermanentBufferState(m_buffers->vertexBuffer, nvrhi::ResourceStates::VertexBuffer);

		auto geometry = std::make_shared<MeshGeometry>();
		geometry->numIndices = dim(g_Indices);
		geometry->numVertices = dim(g_Positions);

		m_meshInfo = std::make_shared<MeshInfo>();
		m_meshInfo->name = u8"CubeMesh";
		m_meshInfo->buffers = m_buffers;
		m_meshInfo->totalIndices = geometry->numIndices;
		m_meshInfo->totalVertices = geometry->numVertices;
		m_meshInfo->geometries.push_back(geometry);

		m_sceneGraph = std::make_shared<SceneGraph>();
		auto node = std::make_shared<SceneGraphNode>();

		m_cubeConstantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(ConstantBufferEntry), "ConstantBuffer")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));
			
		//Load("asset/Robot.gltf", node, m_rootFileSystem);

		Load("asset/cubes.gltf", node, m_rootFileSystem);

		//auto node2 = std::make_shared<SceneGraphNode>();
		//Load("asset/gyroscope.gltf", node2, m_rootFileSystem);
		//
		m_sceneGraph->SetRootNode(node);
		//m_sceneGraph->Attach(node, node2);

		//std::vector<GuGuUtf8Str> nodeNames;
		//SceneGraphWalker walker(m_sceneGraph->GetRootNode().get());
		//while (walker)
		//{
		//	auto current = walker.Get();
		//	auto parent = current->GetParent();
		//
		//	//name
		//	nodeNames.push_back(current->m_name);
		//	walker.Next(true);
		//}
		m_uiData->nodeNames.push_back(m_sceneGraph->GetRootNode()->GetName());
		//SceneGraphNode* current = node.get();
		//if (current)
		//{
		//	m_uiData->nodeNames.push_back(current->GetName());
		//	SceneGraphNode* brother = nullptr;
		//	for (brother = current->GetNextSibling(); brother; brother = brother->GetNextSibling())
		//	{
		//		m_uiData->nodeNames.push_back(brother->GetName());
		//	}
		//}


		m_uiData->getNodeChildrens = std::bind(&Demo::getNodeChildrens, this, std::placeholders::_1, std::placeholders::_2);

		m_uiData->selectionChanged = std::bind(&Demo::selectionChanged, this, std::placeholders::_1, std::placeholders::_2);

		//refresh scene graph
		for (const auto& mesh : m_sceneGraph->GetMeshes())
		{
			auto buffers = mesh->buffers;

			if (!buffers)
				continue;

			if (!buffers->indexData.empty() && !buffers->indexBuffer)
			{
				nvrhi::BufferDesc bufferDesc;
				bufferDesc.isIndexBuffer = true;
				bufferDesc.byteSize = buffers->indexData.size() * sizeof(uint32_t);
				bufferDesc.debugName = "IndexBuffer";
				bufferDesc.canHaveTypedViews = true;
				bufferDesc.canHaveRawViews = true;
				bufferDesc.format = nvrhi::Format::R32_UINT;
				//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;

				buffers->indexBuffer = GetDevice()->createBuffer(bufferDesc);

				m_CommandList->beginTrackingBufferState(buffers->indexBuffer, nvrhi::ResourceStates::Common);

				m_CommandList->writeBuffer(buffers->indexBuffer, buffers->indexData.data(), buffers->indexData.size() * sizeof(uint32_t));
				std::vector<uint32_t>().swap(buffers->indexData);

				nvrhi::ResourceStates state = nvrhi::ResourceStates::IndexBuffer | nvrhi::ResourceStates::ShaderResource;

				m_CommandList->setPermanentBufferState(buffers->indexBuffer, state);
				m_CommandList->commitBarriers();
			}

			if (!buffers->vertexBuffer)
			{
				nvrhi::BufferDesc bufferDesc;
				bufferDesc.isVertexBuffer = true;
				bufferDesc.byteSize = 0;
				bufferDesc.debugName = "VertexBuffer";
				bufferDesc.canHaveTypedViews = true;
				bufferDesc.canHaveRawViews = true;

				if (!buffers->positionData.empty())
				{
					AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::Position),
						buffers->positionData.size() * sizeof(buffers->positionData[0]), bufferDesc.byteSize);
				}

				if (!buffers->normalData.empty())
				{
					AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::Normal),
						buffers->normalData.size() * sizeof(buffers->normalData[0]), bufferDesc.byteSize);
				}

				if (!buffers->tangentData.empty())
				{
					AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::Tangent),
						buffers->tangentData.size() * sizeof(buffers->tangentData[0]), bufferDesc.byteSize);
				}

				if (!buffers->texCoord1Data.empty())
				{
					AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::TexCoord1),
						buffers->texCoord1Data.size() * sizeof(buffers->texCoord1Data[0]), bufferDesc.byteSize);
				}

				if (!buffers->weightData.empty())
				{
					AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointWeights),
						buffers->weightData.size() * sizeof(buffers->weightData[0]), bufferDesc.byteSize);
				}

				if (!buffers->jointData.empty())
				{
					AppendBufferRange(buffers->getVertexBufferRange(VertexAttribute::JointIndices),
						buffers->jointData.size() * sizeof(buffers->jointData[0]), bufferDesc.byteSize);
				}

				buffers->vertexBuffer = GetDevice()->createBuffer(bufferDesc);

				m_CommandList->beginTrackingBufferState(buffers->vertexBuffer, nvrhi::ResourceStates::Common);

				if (!buffers->positionData.empty())
				{
					const auto& range = buffers->getVertexBufferRange(VertexAttribute::Position);
					m_CommandList->writeBuffer(buffers->vertexBuffer, buffers->positionData.data(), range.byteSize, range.byteOffset);
					std::vector<math::float3>().swap(buffers->positionData);
				}

				if (!buffers->normalData.empty())
				{
					const auto& range = buffers->getVertexBufferRange(VertexAttribute::Normal);
					m_CommandList->writeBuffer(buffers->vertexBuffer, buffers->normalData.data(), range.byteSize, range.byteOffset);
					std::vector<uint32_t>().swap(buffers->normalData);
				}

				if (!buffers->tangentData.empty())
				{
					const auto& range = buffers->getVertexBufferRange(VertexAttribute::Tangent);
					m_CommandList->writeBuffer(buffers->vertexBuffer, buffers->tangentData.data(), range.byteSize, range.byteOffset);
					std::vector<uint32_t>().swap(buffers->tangentData);
				}

				if (!buffers->texCoord1Data.empty())
				{
					const auto& range = buffers->getVertexBufferRange(VertexAttribute::TexCoord1);
					m_CommandList->writeBuffer(buffers->vertexBuffer, buffers->texCoord1Data.data(), range.byteSize, range.byteOffset);
					std::vector<math::float2>().swap(buffers->texCoord1Data);
				}

				if (!buffers->weightData.empty())
				{
					const auto& range = buffers->getVertexBufferRange(VertexAttribute::JointWeights);
					m_CommandList->writeBuffer(buffers->vertexBuffer, buffers->weightData.data(), range.byteSize, range.byteOffset);
					std::vector<math::float4>().swap(buffers->weightData);
				}

				if (!buffers->jointData.empty())
				{
					const auto& range = buffers->getVertexBufferRange(VertexAttribute::JointIndices);
					m_CommandList->writeBuffer(buffers->vertexBuffer, buffers->jointData.data(), range.byteSize, range.byteOffset);
					std::vector<math::vector<uint16_t, 4>>().swap(buffers->jointData);
				}

				nvrhi::ResourceStates state = nvrhi::ResourceStates::VertexBuffer | nvrhi::ResourceStates::ShaderResource;

				m_CommandList->setPermanentBufferState(buffers->vertexBuffer, state);
				m_CommandList->commitBarriers();
			}
		}

		for (const auto& skinnedInstance : m_sceneGraph->GetSkinnedMeshInstances())
		{
			const auto& skinnedMesh = skinnedInstance->GetMesh();

			if (!skinnedMesh->buffers)
			{
				skinnedMesh->buffers = std::make_shared<BufferGroup>();

				uint32_t totalVertices = skinnedMesh->totalVertices;

				skinnedMesh->buffers->indexBuffer = skinnedInstance->GetPrototypeMesh()->buffers->indexBuffer;
				//skinnedMesh->buffers->indexBufferDescriptor = skinnedInstance->GetPrototypeMesh()->buffers->indexBufferDescriptor;

				const auto& prototypeBuffers = skinnedInstance->GetPrototypeMesh()->buffers;
				const auto& skinnedBuffers = skinnedMesh->buffers;

				uint64_t skinnedVertexBufferSize = 0;
				assert(prototypeBuffers->hasAttribute(VertexAttribute::Position));

				AppendBufferRange(skinnedBuffers->getVertexBufferRange(VertexAttribute::Position),
					totalVertices * sizeof(math::float3), skinnedVertexBufferSize);

				//AppendBufferRange(skinnedBuffers->getVertexBufferRange(VertexAttribute::PrevPosition),
				//	totalVertices * sizeof(math::float3), skinnedVertexBufferSize);

				if (prototypeBuffers->hasAttribute(VertexAttribute::Normal))
				{
					AppendBufferRange(skinnedBuffers->getVertexBufferRange(VertexAttribute::Normal),
						totalVertices * sizeof(uint32_t), skinnedVertexBufferSize);
				}

				if (prototypeBuffers->hasAttribute(VertexAttribute::Tangent))
				{
					AppendBufferRange(skinnedBuffers->getVertexBufferRange(VertexAttribute::Tangent),
						totalVertices * sizeof(uint32_t), skinnedVertexBufferSize);
				}

				if (prototypeBuffers->hasAttribute(VertexAttribute::TexCoord1))
				{
					AppendBufferRange(skinnedBuffers->getVertexBufferRange(VertexAttribute::TexCoord1),
						totalVertices * sizeof(math::float2), skinnedVertexBufferSize);
				}

				//if (prototypeBuffers->hasAttribute(VertexAttribute::TexCoord2))
				//{
				//	AppendBufferRange(skinnedBuffers->getVertexBufferRange(VertexAttribute::TexCoord2),
				//		totalVertices * sizeof(float2), skinnedVertexBufferSize);
				//}

				nvrhi::BufferDesc bufferDesc;
				bufferDesc.isVertexBuffer = true;
				bufferDesc.byteSize = skinnedVertexBufferSize;
				bufferDesc.debugName = "SkinnedVertexBuffer";
				bufferDesc.canHaveTypedViews = true;
				bufferDesc.canHaveRawViews = true;
				bufferDesc.canHaveUAVs = true;
				//bufferDesc.isAccelStructBuildInput = m_RayTracingSupported;
				bufferDesc.keepInitialState = true;
				bufferDesc.initialState = nvrhi::ResourceStates::VertexBuffer;

				skinnedBuffers->vertexBuffer = GetDevice()->createBuffer(bufferDesc);

				//if (m_DescriptorTable)
				//{
				//	skinnedBuffers->vertexBufferDescriptor = std::make_shared<DescriptorHandle>(
				//		m_DescriptorTable->CreateDescriptorHandle(nvrhi::BindingSetItem::RawBuffer_SRV(0, skinnedBuffers->vertexBuffer)));
				//}
			}

			if (!skinnedInstance->jointBuffer)
			{
				nvrhi::BufferDesc jointBufferDesc;
				jointBufferDesc.debugName = "JointBuffer";
				jointBufferDesc.initialState = nvrhi::ResourceStates::ConstantBuffer;
				jointBufferDesc.keepInitialState = true;
				jointBufferDesc.canHaveRawViews = true;
				jointBufferDesc.isConstantBuffer = true;
				jointBufferDesc.byteSize = sizeof(dm::float4x4) * skinnedInstance->joints.size();
				skinnedInstance->jointBuffer = GetDevice()->createBuffer(jointBufferDesc);
			}

			//if (!skinnedInstance->skinningBindingSet)
			//{
			//	const auto& prototypeBuffers = skinnedInstance->GetPrototypeMesh()->buffers;
			//	const auto& skinnedBuffers = skinnedInstance->GetMesh()->buffers;
			//
			//	nvrhi::BindingSetDesc setDesc;
			//	setDesc.bindings = {
			//		nvrhi::BindingSetItem::PushConstants(0, sizeof(SkinningConstants)),
			//		nvrhi::BindingSetItem::RawBuffer_SRV(0, prototypeBuffers->vertexBuffer),
			//		nvrhi::BindingSetItem::RawBuffer_SRV(1, skinnedInstance->jointBuffer),
			//		nvrhi::BindingSetItem::RawBuffer_UAV(0, skinnedBuffers->vertexBuffer)
			//	};
			//
			//	skinnedInstance->skinningBindingSet = m_Device->createBindingSet(setDesc, m_SkinningBindingLayout);
			//}
		}

		//SceneGraphWalker walker(node.get());
		//while (walker)
		//{
		//	auto current = walker.Get();
		//	auto parent = current->GetParent();
		//
		//	current->UpdateLocalTransform();
		//
		//	if (parent)
		//	{
		//		current->m_GlobalTransform = current->m_HasLocalTransform
		//			? current->m_LocalTransform * parent->m_GlobalTransform
		//			: parent->m_GlobalTransform;
		//	}
		//	else
		//	{
		//		current->m_GlobalTransform = current->m_LocalTransform;
		//	}
		//	current->m_GlobalTransformFloat = dm::affine3(current->m_GlobalTransform);
		//
		//	walker.Next(true);
		//}

		m_meshInstance = std::make_shared<MeshInstance>(m_meshInfo);
		//node->SetLeaf(m_meshInstance);
		//node->SetName("CubeNode");

		std::shared_ptr<ShaderFactory> shaderFactory = std::make_shared<ShaderFactory>(GetDevice(), m_rootFileSystem);
		m_commonRenderPass = std::make_shared<CommonRenderPasses>(GetDevice(), shaderFactory);
		m_bindingCache = std::make_shared<BindingCache>(GetDevice());

		std::vector<ShaderMacro> macros;
		macros.push_back({ "DEFAULT", "1" });
		m_VertexShader = shaderFactory->CreateShader("asset/shader/OpaqueGeometry.hlsl", "main_vs", &macros,
			nvrhi::ShaderType::Vertex);
		m_PixelShader = shaderFactory->CreateShader("asset/shader/OpaqueGeometry.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);

		macros.clear();
		macros.push_back({ "SKINNED", "1"});
		m_SkinnedVertexShader = shaderFactory->CreateShader("asset/shader/OpaqueGeometry.hlsl", "main_vs", &macros,
			nvrhi::ShaderType::Vertex);

		m_ConstantBuffers.resize(256); //max objects
		m_PbrMaterialBuffers.resize(256); //max objects
		m_PassBuffers = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(Pass), "PassBuffer")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));
		m_LightBuffers = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(Light), "LightBuffer")
				.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));
		for (size_t i = 0; i < m_ConstantBuffers.size(); ++i)
		{
			m_ConstantBuffers[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(ConstantBufferEntry), "ConstantBuffer")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));

			m_PbrMaterialBuffers[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(PbrMaterial), "PbrMaterialConstantBuffer")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		
		nvrhi::VertexAttributeDesc attributes[] = {
				nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(math::float3)),
				nvrhi::VertexAttributeDesc()
						.setName("UV")
						.setFormat(nvrhi::Format::RG32_FLOAT)
						.setOffset(0)
						.setBufferIndex(1)
						.setElementStride(sizeof(math::float2)),
				nvrhi::VertexAttributeDesc()
						.setName("NORMAL")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(2)
						.setElementStride(sizeof(math::float3)),
				nvrhi::VertexAttributeDesc()
						.setName("TANGENT")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(3)
						.setElementStride(sizeof(math::float3)),
				nvrhi::VertexAttributeDesc()
						.setName("BONEWEIGHTS")
						.setFormat(nvrhi::Format::RGBA32_FLOAT)
						.setOffset(0)
						.setBufferIndex(4)
						.setElementStride(sizeof(math::float4)),
				nvrhi::VertexAttributeDesc()
						.setName("BONEINDICES")
						.setFormat(nvrhi::Format::RGBA16_UINT)
						.setOffset(0)
						.setBufferIndex(5)
						.setElementStride(sizeof(uint16_t) * 4),
		};

		m_InputLayout = GetDevice()->createInputLayout(attributes,
			uint32_t(std::size(attributes)),
			m_VertexShader);

		nvrhi::BindingLayoutDesc layoutDesc;
		layoutDesc.visibility = nvrhi::ShaderType::All;
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0),
			nvrhi::BindingLayoutItem::ConstantBuffer(2),
			nvrhi::BindingLayoutItem::ConstantBuffer(3),
			nvrhi::BindingLayoutItem::ConstantBuffer(4),
			nvrhi::BindingLayoutItem::Texture_SRV(0),
			nvrhi::BindingLayoutItem::Sampler(0)
		};

		m_BindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0),
			nvrhi::BindingLayoutItem::ConstantBuffer(1),
			nvrhi::BindingLayoutItem::ConstantBuffer(2),
			nvrhi::BindingLayoutItem::ConstantBuffer(3),
			nvrhi::BindingLayoutItem::ConstantBuffer(4),
			nvrhi::BindingLayoutItem::Texture_SRV(0),
			nvrhi::BindingLayoutItem::Sampler(0)
		};

		m_SkinnedBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		m_textureCache = TextureCache(GetDevice(), m_rootFileSystem);
		//todo:load texture
		GuGuUtf8Str textureFileName = u8"asset/fun.jpg";
		std::shared_ptr <LoadedTexture> texture = m_textureCache.LoadTextureFromFile(
			textureFileName, true, nullptr, m_CommandList);
		m_Texture = texture->texture;

		auto samplerDesc = nvrhi::SamplerDesc()
			.setAllFilters(false)
			.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
		m_pointWrapSampler = GetDevice()->createSampler(samplerDesc);

		//------grid------
		m_gridVertexShader = shaderFactory->CreateShader("asset/shader/grid.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_gridPixelShader = shaderFactory->CreateShader("asset/shader/grid.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0),
			nvrhi::BindingLayoutItem::ConstantBuffer(1)
		};
		m_gridBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc gridAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(GridVertex)),
				nvrhi::VertexAttributeDesc()
						.setName("TEXCOORD0")
						.setFormat(nvrhi::Format::RG32_FLOAT)
						.setOffset(0)
						.setBufferIndex(1)
						.setElementStride(sizeof(GridVertex)),
		};
		m_gridInputLayout = GetDevice()->createInputLayout(gridAttributes,
			uint32_t(std::size(gridAttributes)),
			m_gridVertexShader);
		m_gridConstantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(GridConstantBufferEntry), "ConstantBuffer")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));

		m_gridPropertiesConstantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(GridProperties), "GridPropertiesConstantBuffer")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));

		m_gridVertices.push_back(GridVertex(math::float3(-500.0f, 0.0f, -500.0f), math::float2(0.0f, 0.0f)));
		m_gridVertices.push_back(GridVertex(math::float3(-500.0f, 0.0f, 500.0f), math::float2(0.0f, 1.0f)));
		m_gridVertices.push_back(GridVertex(math::float3(500.0f, 0.0f, -500.0f), math::float2(1.0f, 0.0f)));
		
		m_gridVertices.push_back(GridVertex(math::float3(-500.0f, 0.0f, 500.0f), math::float2(0.0f, 1.0f)));
		m_gridVertices.push_back(GridVertex(math::float3(500.0f, 0.0f, 500.0f), math::float2(1.0f, 1.0f)));
		m_gridVertices.push_back(GridVertex(math::float3(500.0f, 0.0f, -500.0f), math::float2(1.0f, 0.0f)));
		

		nvrhi::BufferDesc gridVertexbufferDesc;
		gridVertexbufferDesc.isVertexBuffer = true;
		gridVertexbufferDesc.byteSize = sizeof(GridVertex) * m_gridVertices.size();
		gridVertexbufferDesc.debugName = "gridVertexBuffer";
		gridVertexbufferDesc.canHaveTypedViews = true;
		m_gridVertexBuffer = GetDevice()->createBuffer(gridVertexbufferDesc);

		m_CommandList->beginTrackingBufferState(m_gridVertexBuffer,
			nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(m_gridVertexBuffer, m_gridVertices.data(), sizeof(GridVertex) * m_gridVertices.size());
		m_CommandList->setPermanentBufferState(m_gridVertexBuffer,
			nvrhi::ResourceStates::VertexBuffer);//note:will call end tracking buffer state
		//------grid------

		//------gizmos------
		m_gizmosVertexShader = shaderFactory->CreateShader("asset/shader/gizmos.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_gizmosPixelShader = shaderFactory->CreateShader("asset/shader/gizmos.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0),
			nvrhi::BindingLayoutItem::ConstantBuffer(1)
		};
		m_gizmosBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc gizmosAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(math::float3)),
				nvrhi::VertexAttributeDesc()
						.setName("UV")
						.setFormat(nvrhi::Format::RG32_FLOAT)
						.setOffset(0)
						.setBufferIndex(1)
						.setElementStride(sizeof(math::float2)),
				nvrhi::VertexAttributeDesc()
						.setName("NORMAL")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(2)
						.setElementStride(sizeof(math::float3)),
				nvrhi::VertexAttributeDesc()
						.setName("TANGENT")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(3)
						.setElementStride(sizeof(math::float3)),
		};
		m_gizmosInputLayout = GetDevice()->createInputLayout(gizmosAttributes,
			uint32_t(std::size(gizmosAttributes)),
			m_gizmosVertexShader);

		m_gizmosConstantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(GizmosBufferEntry), "GizmosBufferEntryConstantBuffer")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));

		m_gizmosPropertiesConstantBuffers.resize(18);
		for (uint32_t i = 0; i < 18; ++i)
		{
			m_gizmosPropertiesConstantBuffers[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(GizmosPropertiesBuffer), "GizmosPropertiesConstantBuffer")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		//------gizmos------

		//------terrain------
		m_terrainVertexShader = shaderFactory->CreateShader("asset/shader/Terrain.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_terrainPixelShader = shaderFactory->CreateShader("asset/shader/Terrain.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0), //normal transform
			nvrhi::BindingLayoutItem::ConstantBuffer(1), //ambient color
			nvrhi::BindingLayoutItem::Texture_SRV(0), //terrain height
			nvrhi::BindingLayoutItem::Texture_SRV(1), //terrain color1
			nvrhi::BindingLayoutItem::Texture_SRV(2), //terrain color2
			nvrhi::BindingLayoutItem::Texture_SRV(3), //terrain color3
			nvrhi::BindingLayoutItem::Texture_SRV(4), //terrain color4
			nvrhi::BindingLayoutItem::Texture_SRV(5), //blend texture
			nvrhi::BindingLayoutItem::Sampler(0) //sampler
		};
		m_terrainBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc terrainAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(math::float3))
		};
		m_terrainInputLayout = GetDevice()->createInputLayout(terrainAttributes,
			uint32_t(std::size(terrainAttributes)),
			m_terrainVertexShader);
		//------terrain------

		//------water------
		m_waterVertexShader = shaderFactory->CreateShader("asset/shader/water.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_waterPixelShader = shaderFactory->CreateShader("asset/shader/water.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0), //normal transform
			nvrhi::BindingLayoutItem::ConstantBuffer(1), //water properties
			nvrhi::BindingLayoutItem::Texture_SRV(0), //color texture
			nvrhi::BindingLayoutItem::Texture_SRV(1), //dir texture
			nvrhi::BindingLayoutItem::Sampler(0) //sampler
		};
		m_waterBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc waterAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(math::float3))
		};
		m_waterInputLayout = GetDevice()->createInputLayout(waterAttributes,
			uint32_t(std::size(waterAttributes)),
			m_waterVertexShader);
		//------water------

		//------debug draw camera------
		m_cameraConstantBuffer.resize(256);//max objects
		m_cameraPropertiesConstantBuffers.resize(256);//max objects
		for (size_t i = 0; i < m_cameraConstantBuffer.size(); ++i)
		{
			m_cameraConstantBuffer[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(CameraBufferEntry), " CameraBufferEntry")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		for (size_t i = 0; i < m_cameraPropertiesConstantBuffers.size(); ++i)
		{
			m_cameraPropertiesConstantBuffers[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(CameraPropertiesBuffer), " CameraPropertiesBuffer")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		m_cameraVertexShader = shaderFactory->CreateShader("asset/shader/camera.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_cameraPixelShader = shaderFactory->CreateShader("asset/shader/camera.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0), //normal transform
			nvrhi::BindingLayoutItem::ConstantBuffer(1)  //camera properties
		};
		m_cameraBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc cameraAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(math::float3))
		};
		m_cameraInputLayout = GetDevice()->createInputLayout(cameraAttributes,
			uint32_t(std::size(cameraAttributes)),
			m_cameraVertexShader);
		//------debug draw camera------

		//------game ui------
		m_gameUIConstantBuffer.resize(256);//max objects
		m_gameUIPropertiesConstantBuffers.resize(256);//max objects
		for (size_t i = 0; i < m_gameUIConstantBuffer.size(); ++i)
		{
			m_gameUIConstantBuffer[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(GameUIBufferEntry), "GameUIBufferEntry")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		for (size_t i = 0; i < m_gameUIPropertiesConstantBuffers.size(); ++i)
		{
			m_gameUIPropertiesConstantBuffers[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(GameUIPropertiesBuffer), "GameUIPropertiesBuffer")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		m_gameUIVertexShader = shaderFactory->CreateShader("asset/shader/gameUI.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		std::vector<ShaderMacro> UIMacros;
		UIMacros.push_back(ShaderMacro("UI_DEFAULT", "1"));
		m_gameUIDefaultPixelShader = shaderFactory->CreateShader("asset/shader/gameUI.hlsl", "main_ps", &UIMacros,
			nvrhi::ShaderType::Pixel);
		UIMacros.clear();
		UIMacros.push_back(ShaderMacro("UI_FONT", "1"));
		m_gameUIFontPixelShader = shaderFactory->CreateShader("asset/shader/gameUI.hlsl", "main_ps", &UIMacros,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0), //normal transform
			nvrhi::BindingLayoutItem::ConstantBuffer(1),  //camera properties
			nvrhi::BindingLayoutItem::Texture_SRV(0), //ui texture
			nvrhi::BindingLayoutItem::Sampler(0) //sampler
		};
		m_gameUIBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc gameUIAttributes[] = {

			nvrhi::VertexAttributeDesc()
						.setName("UV")
						.setFormat(nvrhi::Format::RG32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(GameUIVertex)),
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(1)
						.setElementStride(sizeof(GameUIVertex)),
			nvrhi::VertexAttributeDesc()
						.setName("COLOR")
						.setFormat(nvrhi::Format::RGBA32_FLOAT)
						.setOffset(0)
						.setBufferIndex(2)
						.setElementStride(sizeof(GameUIVertex)),
		};
		m_gameUIInputLayout = GetDevice()->createInputLayout(gameUIAttributes,
			uint32_t(std::size(gameUIAttributes)),
			m_gameUIVertexShader);
		//------game ui------

		//-----game ui debug info------
		m_gameUIDebugConstantBuffer.resize(256);//max objects
		m_gameUIDebugPropertiesConstantBuffers.resize(256);//max objects
		for (size_t i = 0; i < m_gameUIDebugConstantBuffer.size(); ++i)
		{
			m_gameUIDebugConstantBuffer[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(GameUIDebugBufferEntry), "GameUIDebugBufferEntry")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		for (size_t i = 0; i < m_gameUIDebugPropertiesConstantBuffers.size(); ++i)
		{
			m_gameUIDebugPropertiesConstantBuffers[i] = GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(GameUIPropertiesBuffer), "GameUIDebugPropertiesBuffer")
				.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
						true));
		}
		m_gameUIDebugVertexShader = m_cameraVertexShader;
		m_gameUIDebugDefaultPixelShader = m_cameraPixelShader;
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0), //normal transform
			nvrhi::BindingLayoutItem::ConstantBuffer(1)  //camera properties
		};
		m_gameUIDebugBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc gameUIDebugAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(GameUIDebugVertex))
		};
		m_gameUIDebugInputLayout = GetDevice()->createInputLayout(gameUIDebugAttributes,
			uint32_t(std::size(gameUIDebugAttributes)),
			m_gameUIDebugVertexShader);
		//-----game ui debug info------

		//-----sky box------
		m_skyBoxConstantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(SkyBoxConstantBufferEntry), "SkyBoxConstantBufferEntry")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));
		m_skyBoxVertexShader = shaderFactory->CreateShader("asset/shader/skyBox.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_skyBoxPixelShader = shaderFactory->CreateShader("asset/shader/skyBox.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0), //normal transform
			nvrhi::BindingLayoutItem::Texture_SRV(0), //cube map
			nvrhi::BindingLayoutItem::Sampler(0)
		};
		m_skyBoxBindingLayout = GetDevice()->createBindingLayout(layoutDesc);

		nvrhi::VertexAttributeDesc skyBoxAttributes[] = {
			nvrhi::VertexAttributeDesc()
						.setName("POSITION")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(0)
						.setElementStride(sizeof(math::float3)),
			nvrhi::VertexAttributeDesc()
						.setName("NORMAL")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(1)
						.setElementStride(sizeof(math::float3)),
			nvrhi::VertexAttributeDesc()
						.setName("TANGENT")
						.setFormat(nvrhi::Format::RGB32_FLOAT)
						.setOffset(0)
						.setBufferIndex(2)
						.setElementStride(sizeof(math::float3)),
			nvrhi::VertexAttributeDesc()
						.setName("UV")
						.setFormat(nvrhi::Format::RG32_FLOAT)
						.setOffset(0)
						.setBufferIndex(3)
						.setElementStride(sizeof(math::float2)),
		};
		m_skyBoxInputLayout = GetDevice()->createInputLayout(skyBoxAttributes,
			uint32_t(std::size(skyBoxAttributes)),
			m_skyBoxVertexShader);

		m_cube = m_geometryHelper.createBox(1, 1, 1, 0);

		createVertexBufferAndIndexBuffer(m_cube);
		//-----sky box------

		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);

		//math::uint2 size = math::uint2(1280.0f, 960.0f);
		//if (!m_renderTarget || math::any(m_renderTargetSize != size))
		//{
		//	m_renderTarget = nullptr;
		//
		//	m_renderTargetSize = size;
		//
		//	initRenderTargetAndDepthTarget();
		//}
		m_cylinderMeshComponent = std::make_shared<StaticMeshComponent>();
		GStaticMesh toru = m_geometryHelper.createToru(0.6f, 0.2f, 16, 4);
		m_cylinderMeshComponent->setGStaticMesh(toru);

		return true;
	}
	void Demo::Update(float fElapsedTimeSeconds)
	{
		m_wallClockTime += fElapsedTimeSeconds;
		for (const auto& anim : m_sceneGraph->GetAnimations())
		{
			float duration = anim->GetDuration();
			float integral;
			float animationTime = std::modf(m_wallClockTime / duration, &integral) * duration;
			anim->Apply(animationTime);
		}
	}
	void Demo::BackBufferResizing()
	{
		m_Pipeline = nullptr;
		m_SkinnedPipeline = nullptr;
	}
	void Demo::Render()
	{
		//const nvrhi::FramebufferInfoEx& fbinfo = framebuffer->getFramebufferInfo();


		//if (!m_Pipeline) {
		//	nvrhi::GraphicsPipelineDesc psoDesc;
		//	psoDesc.VS = m_VertexShader;
		//	psoDesc.PS = m_PixelShader;
		//	psoDesc.inputLayout = m_InputLayout;
		//	psoDesc.bindingLayouts = { m_BindingLayout };
		//	psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
		//	psoDesc.renderState.depthStencilState.depthTestEnable = true;
		//	//psoDesc.renderState.rasterState.frontCounterClockwise = false;
		//	m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, m_frameBuffer);
		//}
		//
		//if (!m_SkinnedPipeline) {
		//	nvrhi::GraphicsPipelineDesc psoDesc;
		//	psoDesc.VS = m_SkinnedVertexShader;
		//	psoDesc.PS = m_PixelShader;
		//	psoDesc.inputLayout = m_InputLayout;
		//	psoDesc.bindingLayouts = { m_SkinnedBindingLayout };
		//	psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
		//	psoDesc.renderState.depthStencilState.depthTestEnable = true;
		//	//psoDesc.renderState.rasterState.frontCounterClockwise = false;
		//	m_SkinnedPipeline = GetDevice()->createGraphicsPipeline(psoDesc, m_frameBuffer);
		//}
		//
		//if (!m_gridPipeline)
		//{
		//	nvrhi::GraphicsPipelineDesc psoDesc;
		//	psoDesc.VS = m_gridVertexShader;
		//	psoDesc.PS = m_gridPixelShader;
		//	psoDesc.inputLayout = m_gridInputLayout;
		//	psoDesc.bindingLayouts = { m_gridBindingLayout };
		//	psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
		//	psoDesc.renderState.depthStencilState.depthTestEnable = true;
		//	psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
		//	m_gridPipeline = GetDevice()->createGraphicsPipeline(psoDesc, m_frameBuffer);
		//}
		//
		//m_CommandList->open();
		//
		//SceneGraphWalker walker(m_sceneGraph->GetRootNode().get());
		//while (walker)
		//{
		//	auto current = walker.Get();
		//	auto parent = current->GetParent();
		//
		//	current->UpdateLocalTransform();
		//
		//	if (parent)
		//	{
		//		current->m_GlobalTransform = current->m_HasLocalTransform
		//			? current->m_LocalTransform * parent->m_GlobalTransform
		//			: parent->m_GlobalTransform;
		//	}
		//	else
		//	{
		//		current->m_GlobalTransform = current->m_LocalTransform;
		//	}
		//	current->m_GlobalTransformFloat = dm::affine3(current->m_GlobalTransform);
		//
		//	walker.Next(true);
		//}
		//
		////note:update skinned meshes
		//std::vector<math::float4x4> jointMatrices;
		//for (const auto& skinnedInstance : m_sceneGraph->GetSkinnedMeshInstances())
		//{
		//	jointMatrices.resize(skinnedInstance->joints.size());
		//
		//	math::daffine3 worldToRoot = inverse(skinnedInstance->GetNode()->GetLocalToWorldTransform());
		//
		//	for (size_t i = 0; i < skinnedInstance->joints.size(); ++i)
		//	{
		//		auto jointNode = skinnedInstance->joints[i].node.lock();
		//
		//		math::float4x4 jointMatrix = math::affineToHomogeneous(math::affine3(jointNode->GetLocalToWorldTransform() * worldToRoot));
		//		jointMatrix = skinnedInstance->joints[i].inverseBindMatrix * jointMatrix;
		//		jointMatrices[i] = jointMatrix;
		//		//jointMatrices[i] = math::float4x4::identity();
		//	}
		//
		//	m_CommandList->writeBuffer(skinnedInstance->jointBuffer, jointMatrices.data(), jointMatrices.size() * sizeof(math::float4x4));
		//}
		////nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, Color(0.0f, 0.0f, 0.0f, 1.0f));
		////nvrhi::utils::ClearColorAttachment(m_CommandList, m_frameBuffer, 0, Color(0.2f, 0.3f, 0.7f, 1.0f));
		//m_CommandList->clearDepthStencilTexture(m_depthTarget, nvrhi::AllSubresources, true, 1.0f, true, 0);
		//
		//math::float3 cameraPos = math::float3(0.0f, 0.0f, m_uiData->camPos);
		//math::float3 cameraDir = normalize(math::float3(0.0f, m_uiData->dir, 1.0f) - cameraPos);
		//math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
		//math::float3 cameraRight = normalize(cross(cameraUp, cameraDir));
		//cameraUp = normalize(cross(cameraDir, cameraRight));
		//
		//math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, -cameraPos);
		//
		////ConstantBufferEntry modelConstants;
		//math::affine3 viewMatrix =
		//	math::yawPitchRoll(0.f, math::radians(-30.f), 0.f)
		//	* math::translation(math::float3(0, 0, 2));
		//math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(45.f),
		//	float(fbinfo.width) /
		//	float(fbinfo.height), 1.0f, 400.0f
		//);
		//math::float4x4 viewProjMatrix = math::affineToHomogeneous(worldToView) * projMatrix;
		////modelConstants.viewProjMatrix = viewProjMatrix;
		//
		//Pass pass;
		//pass.camPos = cameraPos;//todo:这里是否是错误的？
		//m_CommandList->writeBuffer(m_PassBuffers, &pass, sizeof(pass));
		//
		//Light light;
		//light.lightPositions[0] = math::float4(-10.0f, 10.0f, -10.0f, 0.0f);
		//light.lightPositions[1] = math::float4(10.0f, 10.0f, -10.0f, 0.0f);
		//light.lightPositions[2] = math::float4(-10.0f, -10.0f, -10.0f, 0.0f);
		//light.lightPositions[3] = math::float4(10.0f, -10.0f, -10.0f, 0.0f);
		//for (size_t i = 0; i < 4; ++i)
		//{
		//	//light.lightPositions[i] = math::float4(10.0f, 10.0f, 10.0f, 0.0f);
		//	//light.lightPositions[i].x += std::sin(Application::getApplication()->getTimer()->GetDeltaTime() * 5.0);
		//	light.lightColors[i] = math::float4(900.0f, 900.0f, 900.0f, 1.0f);
		//}
		//m_CommandList->writeBuffer(m_LightBuffers, &light, sizeof(light));
		//// Upload all constant buffer slices at once.
		////m_CommandList->writeBuffer(m_ConstantBuffer, &modelConstants, sizeof(modelConstants));
		//
		//nvrhi::GraphicsState state;
		//// Pick the right binding set for this view.
		////state.bindings = { bindingSet };
		////state.indexBuffer = { m_buffers->indexBuffer, nvrhi::Format::R32_UINT, 0 };
		//// Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
		////state.vertexBuffers = {
		////		{m_buffers->vertexBuffer, 1, sizeof(math::float2)},
		////		{m_buffers->vertexBuffer, 0, 0}
		////};
		////state.pipeline = m_Pipeline;
		//state.framebuffer = m_frameBuffer;
		//
		//// Construct the viewport so that all viewports form a grid.
		//const float width = float(fbinfo.width);
		//const float height = float(fbinfo.height);
		//const float left = 0;
		//const float top = 0;
		//
		//const nvrhi::Viewport viewport = nvrhi::Viewport(left, left + width, top,
		//	top + height, 0.f, 1.f);
		//state.viewport.addViewportAndScissorRect(viewport);
		//
		//nvrhi::GraphicsState gridGraphicsState;
		//
		//gridGraphicsState.framebuffer = m_frameBuffer;
		//
		//gridGraphicsState.viewport.addViewportAndScissorRect(viewport);
		//
		//RenderView(state, gridGraphicsState, viewProjMatrix);
		//
		////blit to swap chain framebuffer
		////m_commonRenderPass->BlitTexture(m_CommandList, framebuffer, m_renderTarget, m_bindingCache.get());
		//
		//m_CommandList->close();
		//GetDevice()->executeCommandList(m_CommandList);
	}
	void Demo::LoadScene(const GuGuUtf8Str& filePath)
	{
		//load scene from gltf
		m_testGraph = std::make_shared<SceneGraph>();
	}
	nvrhi::TextureHandle Demo::getRenderTarget()
	{
		return m_renderTarget;
	}

	void Demo::renderLevelRuntime(const std::shared_ptr<Level> inLevel, std::shared_ptr<ViewportClient> inViewportClient)
	{
		if (!m_Pipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_VertexShader; //opaque geometry
			psoDesc.PS = m_PixelShader; //opaque geometry
			psoDesc.inputLayout = m_InputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_BindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_SkinnedPipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_SkinnedVertexShader;
			psoDesc.PS = m_PixelShader;
			psoDesc.inputLayout = m_InputLayout;
			psoDesc.bindingLayouts = { m_SkinnedBindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_SkinnedPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_terrainPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_terrainVertexShader; //terrain
			psoDesc.PS = m_terrainPixelShader; //terrain
			psoDesc.inputLayout = m_terrainInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_terrainBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			//psoDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_terrainPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_waterPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_waterVertexShader; //water
			psoDesc.PS = m_waterPixelShader; //water
			psoDesc.inputLayout = m_waterInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_waterBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			//psoDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_waterPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gameUIPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gameUIVertexShader;
			psoDesc.PS = m_gameUIDefaultPixelShader;
			psoDesc.inputLayout = m_gameUIInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_gameUIBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.blendState.targets[0].setSrcBlendAlpha(nvrhi::BlendFactor::Zero);
			psoDesc.renderState.blendState.targets[0].setDestBlendAlpha(nvrhi::BlendFactor::One);
			m_gameUIPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gameUIFontPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gameUIVertexShader;
			psoDesc.PS = m_gameUIFontPixelShader;
			psoDesc.inputLayout = m_gameUIInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_gameUIBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.blendState.targets[0].setSrcBlendAlpha(nvrhi::BlendFactor::Zero);
			psoDesc.renderState.blendState.targets[0].setDestBlendAlpha(nvrhi::BlendFactor::One);
			m_gameUIFontPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_skyBoxPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_skyBoxVertexShader;
			psoDesc.PS = m_skyBoxPixelShader;
			psoDesc.inputLayout = m_skyBoxInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_skyBoxBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.rasterState.frontCounterClockwise = true;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			psoDesc.renderState.depthStencilState.depthWriteEnable = false;
			psoDesc.renderState.depthStencilState.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
			m_skyBoxPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		m_drawItems.clear();

		m_CommandList->open();

		nvrhi::utils::ClearColorAttachment(m_CommandList, inViewportClient->getFramebuffer(), 0, Color(0.2f, 0.3f, 0.7f, 1.0f));
		m_CommandList->clearDepthStencilTexture(inViewportClient->getDepthTarget(), nvrhi::AllSubresources, true, 1.0f, true, 0);

		const std::vector<std::shared_ptr<GameObject>>& gameObjects = inLevel->getGameObjects();
		
		std::shared_ptr<CameraComponent> cameraComponent;
		std::shared_ptr<TransformComponent> cameraTransformComponent;
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			cameraComponent = gameObjects[i]->getComponent<CameraComponent>();
			if (cameraComponent != nullptr)
			{
				cameraTransformComponent = gameObjects[i]->getComponent<TransformComponent>();
				break;
			}
		}

		if (cameraComponent != nullptr)
		{
			math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(cameraComponent->getFov()),
				inViewportClient->getAspectRatio(), cameraComponent->getNearPlane(), cameraComponent->getFarPlane()
			);
			math::float4x4 viewMatrix = math::inverse(math::affineToHomogeneous(cameraTransformComponent->GetLocalToWorldTransformFloat()));
			math::float4x4 viewProjMatrix = viewMatrix * projMatrix;
			//modelConstants.viewProjMatrix = viewProjMatrix;

			Pass pass;
			pass.camPos = math::float3(cameraTransformComponent->getTranslation());
			m_CommandList->writeBuffer(m_PassBuffers, &pass, sizeof(pass));

			uint32_t lightCount = 0;
			Light light;
			std::memset(light.lightPositions, 0, sizeof(light.lightPositions));
			std::memset(light.lightColors, 0, sizeof(light.lightColors));
			//draw level
			const std::vector<std::shared_ptr<GameObject>>& gameObjects = inLevel->getGameObjects();
			for (size_t i = 0; i < gameObjects.size(); ++i)
			{
				std::shared_ptr<LightComponent> lightComponent = gameObjects[i]->getComponent<LightComponent>();
				std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
				if (lightComponent && transformComponent)
				{
					if (lightCount < m_maxLightCounts)
					{
						math::double3 pos = transformComponent->getTranslation();
						light.lightPositions[lightCount] = math::float4(pos.x, pos.y, pos.z, 1.0f);
						light.lightColors[lightCount] = lightComponent->m_lightColor.toFloat4() * math::float4(1000.0f, 1000.0f, 1000.0f, 1.0f);
						++lightCount;
					}
				}
			}

			for (size_t i = 0; i < gameObjects.size(); ++i)
			{
				std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
				std::shared_ptr<StaticMeshComponent> staticMeshComponent = gameObjects[i]->getComponent<StaticMeshComponent>();
				//std::shared_ptr<StaticMeshComponent> staticMeshComponent = m_cylinderMeshComponent;
				std::shared_ptr<MaterialComponent> materialComponent = gameObjects[i]->getComponent<MaterialComponent>();
				//todo:修复这里
				if (transformComponent == nullptr || staticMeshComponent == nullptr || materialComponent == nullptr)
					continue;
				std::shared_ptr<GStaticMesh> staticMesh = staticMeshComponent->getStaticMesh();

				if (staticMesh->m_indexBuffer == nullptr || staticMesh->m_vertexBuffer == nullptr)
				{
					createVertexBufferAndIndexBuffer(*staticMesh);
				}
				if (staticMesh->m_bIsSkeletonMesh && staticMesh->m_finalBoneMatricesBuffer == nullptr)
				{
					//create final bone
					staticMesh->m_finalBoneMatricesBuffer = GetDevice()->createBuffer(
						nvrhi::utils::CreateStaticConstantBufferDesc(
							sizeof(math::float4x4) * 256, "BoneMatrices")
						.setInitialState(
							nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
								true));
				}
				if (materialComponent->m_bufferHandle == nullptr)
				{
					materialComponent->m_bufferHandle = GetDevice()->createBuffer(
						nvrhi::utils::CreateStaticConstantBufferDesc(
							sizeof(PbrMaterial), "PbrMaterialConstantBuffer")
						.setInitialState(
							nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
								true));
				}
				PbrMaterial pbrMaterial;
				pbrMaterial.albedo = materialComponent->m_albedo;
				pbrMaterial.metallic = materialComponent->m_metallic;;
				pbrMaterial.roughness = materialComponent->m_roughness;
				pbrMaterial.ao = materialComponent->m_ao;
				m_CommandList->writeBuffer(materialComponent->m_bufferHandle, &pbrMaterial, sizeof(pbrMaterial));
				if (materialComponent->getAlbedoTexture()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(materialComponent->getAlbedoTexture(), m_commonRenderPass.get(), m_CommandList);
				}

				if (staticMesh->m_bIsSkeletonMesh)
				{
					//write bone matrices
					m_CommandList->writeBuffer(staticMesh->m_finalBoneMatricesBuffer, staticMesh->m_finalBoneMatrices.data(), sizeof(math::float4x4) * staticMesh->m_finalBoneMatrices.size());
				}

				const math::affine3& worldMatrix = transformComponent->GetLocalToWorldTransformFloat();

				DrawItem drawItem;
				drawItem.m_staticMesh = staticMesh.get();
				drawItem.mesh = m_meshInfo.get();
				drawItem.bufferGroup = m_meshInfo->buffers.get();
				drawItem.meshGeometry = m_meshInfo->geometries[0].get();
				drawItem.m_worldMatrix = m_ConstantBuffers[i];
				drawItem.m_pbrMaterial = materialComponent->m_bufferHandle;
				drawItem.m_albedoTexture = materialComponent->getAlbedoTexture()->m_texture;
				drawItem.m_isSkinned = staticMesh.get()->m_bIsSkeletonMesh;
				drawItem.m_skinnedMatrix = staticMesh->m_finalBoneMatricesBuffer;//final bone
				m_drawItems.push_back(drawItem);

				//get the world matrix
				ConstantBufferEntry modelConstants;
				modelConstants.viewProjMatrix = viewProjMatrix;
				modelConstants.worldMatrix = math::affineToHomogeneous(worldMatrix);
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_ConstantBuffers[i], &modelConstants, sizeof(modelConstants));

				//PbrMaterial pbrMaterial;
				//pbrMaterial.albedo = m_uiData->color;
				//pbrMaterial.metallic = m_uiData->metallic;;
				//pbrMaterial.roughness = m_uiData->roughness;
				//pbrMaterial.ao = 0.0f;
				//m_CommandList->writeBuffer(m_PbrMaterialBuffers[index], &pbrMaterial, sizeof(pbrMaterial));
				//++index;
			}
			m_CommandList->writeBuffer(m_LightBuffers, &light, sizeof(light));

			nvrhi::GraphicsState graphicsState;
			// Pick the right binding set for this view.
			//state.bindings = { bindingSet };
			//state.indexBuffer = { m_buffers->indexBuffer, nvrhi::Format::R32_UINT, 0 };
			// Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
			//state.vertexBuffers = {
			//		{m_buffers->vertexBuffer, 1, sizeof(math::float2)},
			//		{m_buffers->vertexBuffer, 0, 0}
			//};
			graphicsState.pipeline = m_Pipeline;
			graphicsState.framebuffer = inViewportClient->getFramebuffer();

			// Construct the viewport so that all viewports form a grid.
			const float width = inViewportClient->getViewportSize().x;
			const float height = inViewportClient->getViewportSize().y;
			const float left = 0;
			const float top = 0;

			const nvrhi::Viewport viewport = nvrhi::Viewport(left, left + width, top,
				top + height, 0.f, 1.f);
			graphicsState.viewport.addViewportAndScissorRect(viewport);

			for (size_t i = 0; i < m_drawItems.size(); ++i)
			{
				nvrhi::BindingSetDesc desc;

				nvrhi::BindingSetHandle bindingSet;
				if (!m_drawItems[i].m_isSkinned)
				{
					desc.bindings = {
						nvrhi::BindingSetItem::ConstantBuffer(0, m_drawItems[i].m_worldMatrix),
						nvrhi::BindingSetItem::ConstantBuffer(2, m_drawItems[i].m_pbrMaterial),
						nvrhi::BindingSetItem::ConstantBuffer(3, m_LightBuffers),
						nvrhi::BindingSetItem::ConstantBuffer(4, m_PassBuffers),
						nvrhi::BindingSetItem::Texture_SRV(0, m_drawItems[i].m_albedoTexture),
						nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
					};
					bindingSet = GetDevice()->createBindingSet(desc, m_BindingLayout);
				}
				else
				{
					desc.bindings = {
						nvrhi::BindingSetItem::ConstantBuffer(0, m_drawItems[i].m_worldMatrix),
						nvrhi::BindingSetItem::ConstantBuffer(1, m_drawItems[i].m_skinnedMatrix),
						nvrhi::BindingSetItem::ConstantBuffer(2, m_drawItems[i].m_pbrMaterial),
						nvrhi::BindingSetItem::ConstantBuffer(3, m_LightBuffers),
						nvrhi::BindingSetItem::ConstantBuffer(4, m_PassBuffers),
						nvrhi::BindingSetItem::Texture_SRV(0, m_drawItems[i].m_albedoTexture),
						nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
					};
					bindingSet = GetDevice()->createBindingSet(desc, m_SkinnedBindingLayout);
				}

				graphicsState.bindings = { bindingSet };

				if (!m_drawItems[i].m_staticMesh->m_indexBuffer)
					continue;

				graphicsState.vertexBuffers = {
							{m_drawItems[i].m_staticMesh->m_vertexBuffer, 0, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::Position).byteOffset},
							{m_drawItems[i].m_staticMesh->m_vertexBuffer, 1, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::TexCoord1).byteOffset},
							{m_drawItems[i].m_staticMesh->m_vertexBuffer, 2, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::Normal).byteOffset},
							{m_drawItems[i].m_staticMesh->m_vertexBuffer, 3, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::Tangent).byteOffset},
							{m_drawItems[i].m_staticMesh->m_vertexBuffer, 4, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::JointWeights).byteOffset},
							{m_drawItems[i].m_staticMesh->m_vertexBuffer, 5, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::JointIndices).byteOffset}
				};

				graphicsState.indexBuffer = {
					m_drawItems[i].m_staticMesh->m_indexBuffer, nvrhi::Format::R32_UINT, 0
				};

				if (!m_drawItems[i].m_isSkinned)
				{
					graphicsState.pipeline = m_Pipeline;
				}
				else
				{
					graphicsState.pipeline = m_SkinnedPipeline;
				}

				//update the pipeline, bindings, and other state.
				m_CommandList->setGraphicsState(graphicsState);

				// Draw the model.
				nvrhi::DrawArguments args;
				args.vertexCount = m_drawItems[i].m_staticMesh->m_totalIndices;
				args.instanceCount = 1;
				//args.startVertexLocation = m_drawItems[i].mesh->vertexOffset + m_drawItems[i].meshGeometry->vertexOffsetInMesh;
				//args.startIndexLocation = m_drawItems[i].mesh->indexOffset + m_drawItems[i].meshGeometry->indexOffsetInMesh;
				args.startVertexLocation = 0;
				args.startIndexLocation = 0;
				m_CommandList->drawIndexed(args);
			}

			//draw terrain
			nvrhi::GraphicsState terrainGraphicsState;
			terrainGraphicsState.pipeline = m_terrainPipeline;
			terrainGraphicsState.framebuffer = inViewportClient->getFramebuffer();
			terrainGraphicsState.viewport.addViewportAndScissorRect(viewport);
			for (size_t i = 0; i < gameObjects.size(); ++i)
			{
				std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
				std::shared_ptr<TerrainComponent> terrainComponent = gameObjects[i]->getComponent<TerrainComponent>();
				if (transformComponent && terrainComponent)
				{
					if (terrainComponent->m_indexBufferHandle == nullptr || terrainComponent->m_vertexBufferHandle == nullptr)
					{
						createTerrainVertexBufferAndIndexBuffer(terrainComponent);
					}

					if (terrainComponent->getHeightTexture()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(terrainComponent->getHeightTexture(), m_commonRenderPass.get(), m_CommandList);
					}
					if (terrainComponent->getTerrainTexture1()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture1(), m_commonRenderPass.get(), m_CommandList);
					}
					if (terrainComponent->getTerrainTexture2()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture2(), m_commonRenderPass.get(), m_CommandList);
					}
					if (terrainComponent->getTerrainTexture3()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture3(), m_commonRenderPass.get(), m_CommandList);
					}
					if (terrainComponent->getTerrainTexture4()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture4(), m_commonRenderPass.get(), m_CommandList);
					}
					if (terrainComponent->getBlendTexture()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(terrainComponent->getBlendTexture(), m_commonRenderPass.get(), m_CommandList);
					}
					if (!terrainComponent->m_terrainConstantBuffer)
					{
						terrainComponent->m_terrainConstantBuffer = GetDevice()->createBuffer(
							nvrhi::utils::CreateStaticConstantBufferDesc(
								sizeof(TerrainConstantBufferEntry), "TerrainConstantBufferEntry")
							.setInitialState(
								nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
									true));
					}
					if (terrainComponent->m_terrainPropertiesConstantBuffer.empty())
					{
						terrainComponent->m_terrainPropertiesConstantBuffer.resize(terrainComponent->m_terrainCols * terrainComponent->m_terrainRows);
					}
				}
				else
				{
					continue;
				}
				TerrainConstantBufferEntry terrainConstants;
				terrainConstants.viewProjMatrix = viewProjMatrix;
				terrainConstants.worldMatrix = math::affineToHomogeneous(transformComponent->GetLocalToWorldTransformFloat());
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(terrainComponent->m_terrainConstantBuffer, &terrainConstants, sizeof(TerrainConstantBufferEntry));

				math::float2 terrainSize = math::float2((float)terrainComponent->m_cols * (float)terrainComponent->m_tileSize, (float)terrainComponent->m_rows * (float)terrainComponent->m_tileSize);
				math::float2 terrainBeginXZ = math::float2(-(float)terrainComponent->m_terrainCols * terrainSize.x * 0.5f, -(float)terrainComponent->m_terrainRows * terrainSize.y * 0.5f);
				for (uint32_t i = 0; i < terrainComponent->m_terrainRows; ++i)
				{
					for (uint32_t j = 0; j < terrainComponent->m_terrainCols; ++j)
					{
						uint32_t index = i * terrainComponent->m_terrainCols + j;
						if (!terrainComponent->m_terrainPropertiesConstantBuffer[index])
						{
							terrainComponent->m_terrainPropertiesConstantBuffer[index] = GetDevice()->createBuffer(
								nvrhi::utils::CreateStaticConstantBufferDesc(
									sizeof(TerrainPropertiesBuffer), "TerrainPropertiesBuffer")
								.setInitialState(
									nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
										true));
						}
						math::float2 offsetXZ = math::float2(terrainBeginXZ.x + j * terrainSize.x, terrainBeginXZ.y + i * terrainSize.y);
						TerrainPropertiesBuffer propertiesBuffer;
						propertiesBuffer.m_heightScale = terrainComponent->m_heightScale;
						propertiesBuffer.m_beginXZ = terrainBeginXZ;
						propertiesBuffer.m_xzOffset = offsetXZ;
						m_CommandList->writeBuffer(terrainComponent->m_terrainPropertiesConstantBuffer[index], &propertiesBuffer, sizeof(TerrainPropertiesBuffer));
						//draw terrain
						nvrhi::BindingSetHandle terrainBindingSet;
						nvrhi::BindingSetDesc desc;
						//nvrhi::BindingSetHandle bindingSet;
						desc.bindings = {
								nvrhi::BindingSetItem::ConstantBuffer(0, terrainComponent->m_terrainConstantBuffer),
								nvrhi::BindingSetItem::ConstantBuffer(1, terrainComponent->m_terrainPropertiesConstantBuffer[index]),
								nvrhi::BindingSetItem::Texture_SRV(0, terrainComponent->getHeightTexture()->m_texture),
								nvrhi::BindingSetItem::Texture_SRV(1, terrainComponent->getBlendTexture()->m_texture),
								nvrhi::BindingSetItem::Texture_SRV(2, terrainComponent->getTerrainTexture1()->m_texture),
								nvrhi::BindingSetItem::Texture_SRV(3, terrainComponent->getTerrainTexture2()->m_texture),
								nvrhi::BindingSetItem::Texture_SRV(4, terrainComponent->getTerrainTexture3()->m_texture),
								nvrhi::BindingSetItem::Texture_SRV(5, terrainComponent->getTerrainTexture4()->m_texture),
								nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
						};
						terrainBindingSet = GetDevice()->createBindingSet(desc, m_terrainBindingLayout);

						terrainGraphicsState.bindings = { terrainBindingSet };

						terrainGraphicsState.vertexBuffers = {
							{terrainComponent->m_vertexBufferHandle, 0, 0},
						};

						terrainGraphicsState.indexBuffer = {
							terrainComponent->m_indexBufferHandle, nvrhi::Format::R32_UINT, 0
						};

						terrainGraphicsState.setPipeline(m_terrainPipeline);
						m_CommandList->setGraphicsState(terrainGraphicsState);

						// Draw the model.
						nvrhi::DrawArguments args;
						args.vertexCount = terrainComponent->m_indexData.size();
						args.instanceCount = 1;
						args.startVertexLocation = 0;
						args.startIndexLocation = 0;
						m_CommandList->drawIndexed(args);
					}
				}
			}

			//draw water
			nvrhi::GraphicsState waterGraphicsState;
			waterGraphicsState.pipeline = m_waterPipeline;
			waterGraphicsState.framebuffer = inViewportClient->getFramebuffer();
			waterGraphicsState.viewport.addViewportAndScissorRect(viewport);
			for (size_t i = 0; i < gameObjects.size(); ++i)
			{
				std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
				std::shared_ptr<WaterComponent> waterComponent = gameObjects[i]->getComponent<WaterComponent>();
				if (transformComponent && waterComponent)
				{
					if (waterComponent->m_indexBufferHandle == nullptr || waterComponent->m_vertexBufferHandle == nullptr)
					{
						createWaterVertexBufferAndIndexBuffer(waterComponent);
					}

					if (waterComponent->getDirTexture()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(waterComponent->getDirTexture(), m_commonRenderPass.get(), m_CommandList);
					}
					if (waterComponent->getColorTexture()->m_texture == nullptr)
					{
						m_textureCache.FinalizeTexture(waterComponent->getColorTexture(), m_commonRenderPass.get(), m_CommandList);
					}

					if (!waterComponent->m_waterConstantBuffer)
					{
						waterComponent->m_waterConstantBuffer = GetDevice()->createBuffer(
							nvrhi::utils::CreateStaticConstantBufferDesc(
								sizeof(WaterConstantBufferEntry), " WaterConstantBufferEntry")
							.setInitialState(
								nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
									true));
					}
					if (waterComponent->m_waterPropertiesConstantBuffer.empty())
					{
						waterComponent->m_waterPropertiesConstantBuffer.resize(waterComponent->m_waterCols * waterComponent->m_waterRows);
					}
				}
				else
				{
					continue;
				}
				WaterConstantBufferEntry waterConstants;
				waterConstants.viewProjMatrix = viewProjMatrix;
				waterConstants.worldMatrix = math::affineToHomogeneous(transformComponent->GetLocalToWorldTransformFloat());
				waterConstants.time = m_wallClockTime;
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(waterComponent->m_waterConstantBuffer, &waterConstants, sizeof(WaterConstantBufferEntry));

				math::float2 waterSize = math::float2((float)waterComponent->m_rows * (float)waterComponent->m_tileSize, (float)waterComponent->m_cols * (float)waterComponent->m_tileSize);
				math::float2 waterBeginXZ = math::float2(-(float)waterComponent->m_waterRows * waterSize.x * 0.5f, -(float)waterComponent->m_waterCols * waterSize.y * 0.5f);
				for (uint32_t i = 0; i < waterComponent->m_waterRows; ++i)
				{
					for (uint32_t j = 0; j < waterComponent->m_waterCols; ++j)
					{
						uint32_t index = i * waterComponent->m_waterCols + j;
						if (!waterComponent->m_waterPropertiesConstantBuffer[index])
						{
							waterComponent->m_waterPropertiesConstantBuffer[index] = GetDevice()->createBuffer(
								nvrhi::utils::CreateStaticConstantBufferDesc(
									sizeof(WaterPropertiesBuffer), "WaterPropertiesBuffer")
								.setInitialState(
									nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
										true));
						}
						int32_t mixedColorImageWidth = waterComponent->getMixedColorTexture()->m_width;
						int32_t mixedColorImageHeight = waterComponent->getMixedColorTexture()->m_height;
						int32_t waterImagePx = (i / (float)waterComponent->m_waterRows) * mixedColorImageWidth;
						int32_t waterImagePy = ((waterComponent->m_waterCols - j - 1) / (float)waterComponent->m_waterCols) * mixedColorImageHeight;
						math::float3 mixedColor = waterComponent->getMixedColorTexture()->getColor(waterImagePx, waterImagePy);
						if (mixedColor.x >= 30.0f)
						{
							math::float2 offsetXZ = math::float2(waterBeginXZ.x + j * waterSize.x, waterBeginXZ.y + i * waterSize.y);
							WaterPropertiesBuffer propertiesBuffer;
							propertiesBuffer.m_heightScale = 10.0f;
							propertiesBuffer.m_beginXZ = waterBeginXZ;
							propertiesBuffer.m_xzOffset = offsetXZ;
							m_CommandList->writeBuffer(waterComponent->m_waterPropertiesConstantBuffer[index], &propertiesBuffer, sizeof(WaterPropertiesBuffer));
							//draw water
							nvrhi::BindingSetHandle waterBindingSet;
							nvrhi::BindingSetDesc desc;
							//nvrhi::BindingSetHandle bindingSet;
							desc.bindings = {
									nvrhi::BindingSetItem::ConstantBuffer(0, waterComponent->m_waterConstantBuffer),
									nvrhi::BindingSetItem::ConstantBuffer(1, waterComponent->m_waterPropertiesConstantBuffer[index]),
									nvrhi::BindingSetItem::Texture_SRV(0, waterComponent->getColorTexture()->m_texture),
									nvrhi::BindingSetItem::Texture_SRV(1, waterComponent->getDirTexture()->m_texture),
									nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
							};
							waterBindingSet = GetDevice()->createBindingSet(desc, m_waterBindingLayout);

							waterGraphicsState.bindings = { waterBindingSet };

							waterGraphicsState.vertexBuffers = {
								{waterComponent->m_vertexBufferHandle, 0, 0},
							};

							waterGraphicsState.indexBuffer = {
								waterComponent->m_indexBufferHandle, nvrhi::Format::R32_UINT, 0
							};

							waterGraphicsState.setPipeline(m_waterPipeline);
							m_CommandList->setGraphicsState(waterGraphicsState);

							// Draw the model.
							nvrhi::DrawArguments args;
							args.vertexCount = waterComponent->m_indexData.size();
							args.instanceCount = 1;
							args.startVertexLocation = 0;
							args.startIndexLocation = 0;
							m_CommandList->drawIndexed(args);
						}
					}
				}
			}

			//draw skybox
			nvrhi::GraphicsState skyBoxGraphicsState;
			skyBoxGraphicsState.pipeline = m_skyBoxPipeline;
			skyBoxGraphicsState.framebuffer = inViewportClient->getFramebuffer();
			skyBoxGraphicsState.viewport.addViewportAndScissorRect(viewport);
			for (size_t i = 0; i < gameObjects.size(); ++i)
			{
				std::shared_ptr<CubeComponent> cubeComponent = gameObjects[i]->getComponent<CubeComponent>();
				if (cubeComponent)
				{
					if (cubeComponent->m_textureHandle == nullptr || cubeComponent->isDirty())
					{
						cubeComponent->setDirty(false);

						std::vector<std::shared_ptr<GTexture>> skyBoxTextures = {
							cubeComponent->getFrontTexture(),
							cubeComponent->getBackTexture(),
							cubeComponent->getTopTexture(),
							cubeComponent->getBottomTexture(),
							cubeComponent->getRightTexture(),
							cubeComponent->getLeftTexture(),
						};
						nvrhi::TextureHandle resultHandle = m_textureCache.FinalizeCubeMapTexture(skyBoxTextures, m_commonRenderPass.get(), m_CommandList);
						if (resultHandle != nullptr)
							cubeComponent->m_textureHandle = resultHandle;
					}
					//draw sky box
					nvrhi::BindingSetHandle skyBoxBindingSet;
					nvrhi::BindingSetDesc desc;
					//nvrhi::BindingSetHandle bindingSet;
					desc.bindings = {
							nvrhi::BindingSetItem::ConstantBuffer(0, m_skyBoxConstantBuffer),
							nvrhi::BindingSetItem::Texture_SRV(0, cubeComponent->m_textureHandle),
							nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
					};
					m_skyBoxBindingSet = GetDevice()->createBindingSet(desc, m_skyBoxBindingLayout);

					skyBoxGraphicsState.bindings = { m_skyBoxBindingSet };

					skyBoxGraphicsState.vertexBuffers = {
						//{cameraComponent->m_debugCameraFrustumVertexBuffer, 0, 0}
						{m_cube.m_vertexBuffer, 0, 0}
					};

					skyBoxGraphicsState.indexBuffer = {
						//cameraComponent->m_debugCameraFrustumIndexBuffer, nvrhi::Format::R32_UINT, 0
						m_cube.m_indexBuffer, nvrhi::Format::R32_UINT, 0
					};

					math::float4x4 getRidTranslationViewMatrix = math::float4x4::zero();
					for (int32_t row = 0; row < 3; ++row)
					{
						for (int32_t col = 0; col < 3; ++col)
						{
							getRidTranslationViewMatrix[row][col] = viewMatrix[row][col];
						}
					}

					SkyBoxConstantBufferEntry modelConstants;
					modelConstants.viewProjMatrix = getRidTranslationViewMatrix * projMatrix;
					modelConstants.worldMatrix = math::float4x4::identity();;
					modelConstants.camWorldPos = inViewportClient->getCamPos();
					//get the global matrix to fill constant buffer		
					m_CommandList->writeBuffer(m_skyBoxConstantBuffer, &modelConstants, sizeof(modelConstants));

					//CameraPropertiesBuffer propertiesBuffer;
					//propertiesBuffer.color = math::float3(0.73f, 0.90f, 0.44f);
					//m_CommandList->writeBuffer(m_cameraPropertiesConstantBuffers[i], &propertiesBuffer, sizeof(propertiesBuffer));

					skyBoxGraphicsState.setPipeline(m_skyBoxPipeline);
					m_CommandList->setGraphicsState(skyBoxGraphicsState);

					//draw the model
					nvrhi::DrawArguments args;
					args.vertexCount = 36;
					args.instanceCount = 1;
					args.startVertexLocation = 0;
					args.startIndexLocation = 0;
					m_CommandList->drawIndexed(args);
					break;
				}
			}

			//------draw game ui------
			std::vector<std::shared_ptr<UIDrawInfo>> uiDrawInfos;
			for (size_t i = 0; i < gameObjects.size(); ++i)
			{
				std::shared_ptr<UITransformComponent> transformComponent = gameObjects[i]->getComponent<UITransformComponent>();
				std::vector<std::shared_ptr<Component>> uiComponent = gameObjects[i]->getComponentParentClassIs<UIComponent>();

				for (size_t j = 0; j < uiComponent.size(); ++j)
				{
					std::shared_ptr<UIComponent> currentUIComponent = std::static_pointer_cast<UIComponent>(uiComponent[j]);

					if (currentUIComponent->GetType() == typeof(TextComponent))
					{
						//update atlas
						std::shared_ptr<GFont> font = std::static_pointer_cast<TextComponent>(currentUIComponent)->getFont();
						if (font->m_atlas->isNeedToUpdateAtlas())
						{
							updateAtlas(font->m_atlas);
						}
					}

					std::shared_ptr<UIDrawInfo> drawInfo = currentUIComponent->generateUIDrawInformation();
					if (transformComponent && currentUIComponent)
					{
						uiDrawInfos.push_back(drawInfo);
						//createDebugCameraFrustum(cameraComponent, transformComponent, inViewportClient);
					}
				}
			}

			//sort ui draw infos
			std::sort(uiDrawInfos.begin(), uiDrawInfos.end(), [](const std::shared_ptr<UIDrawInfo>& lhs, const std::shared_ptr<UIDrawInfo>& rhs) {
				return lhs->m_zorder > rhs->m_zorder;
			});

            //GuGu_LOGD("size of ui draw infos, %d", uiDrawInfos.size());

			nvrhi::GraphicsState gameUIGraphicsState;
			gameUIGraphicsState.framebuffer = inViewportClient->getFramebuffer();
			gameUIGraphicsState.viewport.addViewportAndScissorRect(viewport);

			for (int32_t i = 0; i < uiDrawInfos.size(); ++i)
			{
				std::shared_ptr<UIDrawInfo> drawInfo = uiDrawInfos[i];
				if (drawInfo->m_drawType == UIDrawType::Font)
				{
					gameUIGraphicsState.pipeline = m_gameUIFontPipeline;
				}
				else
				{
					gameUIGraphicsState.pipeline = m_gameUIPipeline;
				}
				createUIVertexBufferAndIndexBuffer(drawInfo);

				if (drawInfo->m_texture->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(drawInfo->m_texture, m_commonRenderPass.get(), m_CommandList);
				}

				//draw
				nvrhi::BindingSetHandle uiBindingSet;
				nvrhi::BindingSetDesc desc;
				//nvrhi::BindingSetHandle bindingSet;
				desc.bindings = {
						nvrhi::BindingSetItem::ConstantBuffer(0, m_gameUIConstantBuffer[i]),
						nvrhi::BindingSetItem::ConstantBuffer(1, m_gameUIPropertiesConstantBuffers[i]),
						nvrhi::BindingSetItem::Texture_SRV(0, drawInfo->m_texture->m_texture),
						nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
				};
				uiBindingSet = GetDevice()->createBindingSet(desc, m_gameUIBindingLayout);

				gameUIGraphicsState.bindings = { uiBindingSet };

				gameUIGraphicsState.vertexBuffers = {
					{ drawInfo->m_uiVertexHandle, 0, offsetof(GameUIVertex, m_uv)},
					{ drawInfo->m_uiVertexHandle, 1, offsetof(GameUIVertex, m_position)},
					{ drawInfo->m_uiVertexHandle, 2, offsetof(GameUIVertex, m_color)},
				};

				gameUIGraphicsState.indexBuffer = {
					drawInfo->m_uiIndexHandle, nvrhi::Format::R32_UINT, 0
				};

				GameUIBufferEntry modelConstants;
				//------ortho camera------
				math::float3 cameraPos = math::float3(0.0f, 0.0f, 0.0f);
				math::float3 cameraDir = normalize(math::float3(0.0f, 0.0f, 1.0f));
				math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
				math::float3 cameraRight = normalize(cross(cameraUp, cameraDir));
				cameraUp = normalize(cross(cameraDir, cameraRight));
				math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, 0.0f);
				//------ortho camera------
				//float inverseScale = 1.0f / transformComponent->getScaleFactor();
				modelConstants.viewProjMatrix = math::affineToHomogeneous(worldToView) * math::orthoProjD3DStyle(0, width, 0, height, 0, 1);
				modelConstants.worldMatrix = math::float4x4::identity();
				modelConstants.camWorldPos = inViewportClient->getCamPos();
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_gameUIConstantBuffer[i], &modelConstants, sizeof(modelConstants));

				GameUIPropertiesBuffer propertiesBuffer;
				propertiesBuffer.color = math::float3(1.0f, 1.0f, 1.0f);
				m_CommandList->writeBuffer(m_gameUIPropertiesConstantBuffers[i], &propertiesBuffer, sizeof(propertiesBuffer));

				//gameUIGraphicsState.setPipeline(m_gameUIPipeline);
				m_CommandList->setGraphicsState(gameUIGraphicsState);

				//draw the model
				nvrhi::DrawArguments args;
				args.vertexCount = drawInfo->m_uiIndices.size();
				args.instanceCount = 1;
				args.startVertexLocation = 0;
				args.startIndexLocation = 0;
				m_CommandList->drawIndexed(args);
			}
			//------draw game ui------
		}

		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);
	}

	void Demo::renderLevel(const std::shared_ptr<Level> inLevel, std::shared_ptr<ViewportClient> inViewportClient)
	{
		//math::uint2 intViewportSize(viewportSize.x, viewportSize.y);
		//if (math::any(m_renderTargetSize != intViewportSize))
		//{
		//	m_renderTarget = nullptr;
		//
		//	m_renderTargetSize = intViewportSize;
		//
		//	initRenderTargetAndDepthTarget();
		//}

		if (!m_Pipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_VertexShader; //opaque geometry
			psoDesc.PS = m_PixelShader; //opaque geometry
			psoDesc.inputLayout = m_InputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_BindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_SkinnedPipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_SkinnedVertexShader;
			psoDesc.PS = m_PixelShader;
			psoDesc.inputLayout = m_InputLayout;
			psoDesc.bindingLayouts = { m_SkinnedBindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_SkinnedPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gizmosPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gizmosVertexShader;
			psoDesc.PS = m_gizmosPixelShader;
			psoDesc.inputLayout = m_gizmosInputLayout;
			psoDesc.bindingLayouts = { m_gizmosBindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			nvrhi::BlendState::RenderTarget renderTargetBlendState;
			renderTargetBlendState.blendEnable = true;
			renderTargetBlendState.srcBlend = nvrhi::BlendFactor::SrcAlpha;
			renderTargetBlendState.destBlend = nvrhi::BlendFactor::InvSrcAlpha;
			renderTargetBlendState.blendOp = nvrhi::BlendOp::Add;
			//renderTargetBlendState.srcBlendAlpha = nvrhi::BlendFactor::One;
			//renderTargetBlendState.destBlendAlpha = nvrhi::BlendFactor::Zero;
			//renderTargetBlendState.blendOpAlpha = nvrhi::BlendOp::Add;
			psoDesc.renderState.blendState = nvrhi::BlendState().setRenderTarget(0, renderTargetBlendState);
			psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
			m_gizmosPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_terrainPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_terrainVertexShader; //terrain
			psoDesc.PS = m_terrainPixelShader; //terrain
			psoDesc.inputLayout = m_terrainInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_terrainBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			//psoDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_terrainPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_waterPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_waterVertexShader; //water
			psoDesc.PS = m_waterPixelShader; //water
			psoDesc.inputLayout = m_waterInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_waterBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			//psoDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_waterPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_cameraPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_cameraVertexShader; //camera
			psoDesc.PS = m_cameraPixelShader; //camera
			psoDesc.inputLayout = m_cameraInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_cameraBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::LineList;
			psoDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Wireframe;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			//psoDesc.renderState.rasterState.frontCounterClockwise = false;
			m_cameraPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gridPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gridVertexShader;
			psoDesc.PS = m_gridPixelShader;
			psoDesc.inputLayout = m_gridInputLayout;
			psoDesc.bindingLayouts = { m_gridBindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.blendState.targets[0].setSrcBlendAlpha(nvrhi::BlendFactor::Zero);
			psoDesc.renderState.blendState.targets[0].setDestBlendAlpha(nvrhi::BlendFactor::One);
			psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
			m_gridPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gameUIPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gameUIVertexShader;
			psoDesc.PS = m_gameUIDefaultPixelShader;
			psoDesc.inputLayout = m_gameUIInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_gameUIBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.blendState.targets[0].setSrcBlendAlpha(nvrhi::BlendFactor::Zero);
			psoDesc.renderState.blendState.targets[0].setDestBlendAlpha(nvrhi::BlendFactor::One);
			m_gameUIPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gameUIFontPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gameUIVertexShader;
			psoDesc.PS = m_gameUIFontPixelShader;
			psoDesc.inputLayout = m_gameUIInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_gameUIBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.blendState.targets[0].setSrcBlendAlpha(nvrhi::BlendFactor::Zero);
			psoDesc.renderState.blendState.targets[0].setDestBlendAlpha(nvrhi::BlendFactor::One);
			m_gameUIFontPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_gameUIDebugPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_gameUIDebugVertexShader;
			psoDesc.PS = m_gameUIDebugDefaultPixelShader;
			psoDesc.inputLayout = m_gameUIDebugInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_gameUIDebugBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;	
			m_gameUIDebugPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		if (!m_skyBoxPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_skyBoxVertexShader;
			psoDesc.PS = m_skyBoxPixelShader;
			psoDesc.inputLayout = m_skyBoxInputLayout; //顶点属性
			psoDesc.bindingLayouts = { m_skyBoxBindingLayout }; //constant buffer 这些
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.rasterState.frontCounterClockwise = true;
			psoDesc.renderState.depthStencilState.depthTestEnable = true;
			psoDesc.renderState.depthStencilState.depthWriteEnable = false;
			psoDesc.renderState.depthStencilState.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
			m_skyBoxPipeline = GetDevice()->createGraphicsPipeline(psoDesc, inViewportClient->getFramebuffer());
		}

		m_drawItems.clear();

		m_CommandList->open();
		
		nvrhi::utils::ClearColorAttachment(m_CommandList, inViewportClient->getFramebuffer(), 0, Color(0.2f, 0.3f, 0.7f, 1.0f));
		m_CommandList->clearDepthStencilTexture(inViewportClient->getDepthTarget(), nvrhi::AllSubresources, true, 1.0f, true, 0);
		//math::float3 cameraPos = math::float3(0.0f, 0.0f, m_uiData->camPos);
		//math::float3 cameraDir = normalize(math::float3(0.0f, m_uiData->dir, 1.0f) - cameraPos);
		//math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
		//math::float3 cameraRight = normalize(cross(cameraUp, cameraDir));
		//cameraUp = normalize(cross(cameraDir, cameraRight));
		//
		//math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, -cameraPos);

		//ConstantBufferEntry modelConstants;
		//math::affine3 viewMatrix =
		//math::yawPitchRoll(0.f, math::radians(-30.f), 0.f)
		//* math::translation(math::float3(0, 0, 2));
		math::float4x4 projMatrix = inViewportClient->getPespectiveMatrix();
		math::float4x4 viewProjMatrix = inViewportClient->getWorldToViewMatrix() * projMatrix;
		//modelConstants.viewProjMatrix = viewProjMatrix;

		Pass pass;
		pass.camPos = inViewportClient->getCamPos();
		m_CommandList->writeBuffer(m_PassBuffers, &pass, sizeof(pass));

		uint32_t lightCount = 0;
		Light light;
		std::memset(light.lightPositions, 0, sizeof(light.lightPositions));
		std::memset(light.lightColors, 0, sizeof(light.lightColors));

		//draw level
		const std::vector<std::shared_ptr<GameObject>>& gameObjects = inLevel->getGameObjects();
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<LightComponent> lightComponent = gameObjects[i]->getComponent<LightComponent>();
			std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
			if (lightComponent && transformComponent)
			{	
				if (lightCount < m_maxLightCounts)
				{
					math::double3 pos = transformComponent->getTranslation();
					light.lightPositions[lightCount] = math::float4(pos.x, pos.y, pos.z, 1.0f);
					light.lightColors[lightCount] = lightComponent->m_lightColor.toFloat4() * math::float4(1000.0f, 1000.0f, 1000.0f, 1.0f);
					++lightCount;
				}
			}
		}

		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
			std::shared_ptr<StaticMeshComponent> staticMeshComponent = gameObjects[i]->getComponent<StaticMeshComponent>();
			//std::shared_ptr<StaticMeshComponent> staticMeshComponent = m_cylinderMeshComponent;
			
			std::shared_ptr<MaterialComponent> materialComponent = gameObjects[i]->getComponent<MaterialComponent>();
			//todo:修复这里
			if (transformComponent == nullptr || staticMeshComponent == nullptr || materialComponent == nullptr)
				continue;
			std::shared_ptr<GStaticMesh> staticMesh = staticMeshComponent->getStaticMesh();

			if (staticMesh->m_indexBuffer == nullptr || staticMesh->m_vertexBuffer == nullptr)
			{
				createVertexBufferAndIndexBuffer(*staticMesh);
			}
			if (staticMesh->m_bIsSkeletonMesh && staticMesh->m_finalBoneMatricesBuffer == nullptr)
			{
				//create final bone
				staticMesh->m_finalBoneMatricesBuffer = GetDevice()->createBuffer(
					nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(math::float4x4) * 256, "BoneMatrices")
					.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));
			}
			if (materialComponent->m_bufferHandle == nullptr)
			{
				materialComponent->m_bufferHandle = GetDevice()->createBuffer(
					nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(PbrMaterial), "PbrMaterialConstantBuffer")
					.setInitialState(
					nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));
			}
			PbrMaterial pbrMaterial;
			pbrMaterial.albedo = materialComponent->m_albedo;
			pbrMaterial.metallic = materialComponent->m_metallic;;
			pbrMaterial.roughness = materialComponent->m_roughness;
			pbrMaterial.ao = materialComponent->m_ao;
			m_CommandList->writeBuffer(materialComponent->m_bufferHandle, &pbrMaterial, sizeof(pbrMaterial));
			if (materialComponent->getAlbedoTexture()->m_texture == nullptr)
			{
				m_textureCache.FinalizeTexture(materialComponent->getAlbedoTexture(), m_commonRenderPass.get(), m_CommandList);
			}

			if (staticMesh->m_bIsSkeletonMesh)
			{
				//write bone matrices
				m_CommandList->writeBuffer(staticMesh->m_finalBoneMatricesBuffer, staticMesh->m_finalBoneMatrices.data(), sizeof(math::float4x4) * staticMesh->m_finalBoneMatrices.size());
			}
			
			const math::affine3& worldMatrix = transformComponent->GetLocalToWorldTransformFloat();

			DrawItem drawItem;
			drawItem.m_staticMesh = staticMesh.get();
			drawItem.mesh = m_meshInfo.get();
			drawItem.bufferGroup = m_meshInfo->buffers.get();
			drawItem.meshGeometry = m_meshInfo->geometries[0].get();
			drawItem.m_worldMatrix = m_ConstantBuffers[i];
			drawItem.m_pbrMaterial = materialComponent->m_bufferHandle;
			drawItem.m_albedoTexture = materialComponent->getAlbedoTexture()->m_texture;
			drawItem.m_isSkinned = staticMesh.get()->m_bIsSkeletonMesh;
			drawItem.m_skinnedMatrix = staticMesh->m_finalBoneMatricesBuffer;//final bone
			m_drawItems.push_back(drawItem);

			//get the world matrix
			ConstantBufferEntry modelConstants;
			modelConstants.viewProjMatrix = viewProjMatrix;
			modelConstants.worldMatrix = math::affineToHomogeneous(worldMatrix);
			//get the global matrix to fill constant buffer		
			m_CommandList->writeBuffer(m_ConstantBuffers[i], &modelConstants, sizeof(modelConstants));

			//PbrMaterial pbrMaterial;
			//pbrMaterial.albedo = m_uiData->color;
			//pbrMaterial.metallic = m_uiData->metallic;;
			//pbrMaterial.roughness = m_uiData->roughness;
			//pbrMaterial.ao = 0.0f;
			//m_CommandList->writeBuffer(m_PbrMaterialBuffers[index], &pbrMaterial, sizeof(pbrMaterial));
			//++index;
		}

		if(lightCount > 0)
			m_CommandList->writeBuffer(m_LightBuffers, &light, sizeof(light));

		nvrhi::GraphicsState graphicsState;
		// Pick the right binding set for this view.
		//state.bindings = { bindingSet };
		//state.indexBuffer = { m_buffers->indexBuffer, nvrhi::Format::R32_UINT, 0 };
		// Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
		//state.vertexBuffers = {
		//		{m_buffers->vertexBuffer, 1, sizeof(math::float2)},
		//		{m_buffers->vertexBuffer, 0, 0}
		//};
		graphicsState.pipeline = m_Pipeline;
		graphicsState.framebuffer = inViewportClient->getFramebuffer();

		// Construct the viewport so that all viewports form a grid.
		const float width = inViewportClient->getViewportSize().x;
		const float height = inViewportClient->getViewportSize().y;
		const float left = 0;
		const float top = 0;

		const nvrhi::Viewport viewport = nvrhi::Viewport(left, left + width, top,
			top + height, 0.f, 1.f);
		graphicsState.viewport.addViewportAndScissorRect(viewport);

		for (size_t i = 0; i < m_drawItems.size(); ++i)
		{
			nvrhi::BindingSetDesc desc;

			nvrhi::BindingSetHandle bindingSet;
			if (!m_drawItems[i].m_isSkinned)
			{
				desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_drawItems[i].m_worldMatrix),
					nvrhi::BindingSetItem::ConstantBuffer(2, m_drawItems[i].m_pbrMaterial),
					nvrhi::BindingSetItem::ConstantBuffer(3, m_LightBuffers),
					nvrhi::BindingSetItem::ConstantBuffer(4, m_PassBuffers),
					//nvrhi::BindingSetItem::Texture_SRV(0, m_commonRenderPass->m_whiteTexture),
					nvrhi::BindingSetItem::Texture_SRV(0, m_drawItems[i].m_albedoTexture),
					nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
				};
				bindingSet = GetDevice()->createBindingSet(desc, m_BindingLayout);
			}
			else
			{
				desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_drawItems[i].m_worldMatrix),
					nvrhi::BindingSetItem::ConstantBuffer(1, m_drawItems[i].m_skinnedMatrix),
					nvrhi::BindingSetItem::ConstantBuffer(2, m_drawItems[i].m_pbrMaterial),
					nvrhi::BindingSetItem::ConstantBuffer(3, m_LightBuffers),
					nvrhi::BindingSetItem::ConstantBuffer(4, m_PassBuffers),
					//nvrhi::BindingSetItem::Texture_SRV(0, m_commonRenderPass->m_whiteTexture),
					nvrhi::BindingSetItem::Texture_SRV(0, m_drawItems[i].m_albedoTexture),
					nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
				};
				bindingSet = GetDevice()->createBindingSet(desc, m_SkinnedBindingLayout);
			}

			graphicsState.bindings = { bindingSet };

			if (!m_drawItems[i].m_staticMesh->m_indexBuffer)
				continue;

			graphicsState.vertexBuffers = {
						{m_drawItems[i].m_staticMesh->m_vertexBuffer, 0, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::Position).byteOffset},
						{m_drawItems[i].m_staticMesh->m_vertexBuffer, 1, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::TexCoord1).byteOffset},
						{m_drawItems[i].m_staticMesh->m_vertexBuffer, 2, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::Normal).byteOffset},
						{m_drawItems[i].m_staticMesh->m_vertexBuffer, 3, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::Tangent).byteOffset},
						{m_drawItems[i].m_staticMesh->m_vertexBuffer, 4, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::JointWeights).byteOffset},
						{m_drawItems[i].m_staticMesh->m_vertexBuffer, 5, m_drawItems[i].m_staticMesh->getVertexBufferRange(GVertexAttribute::JointIndices).byteOffset}
			};

			graphicsState.indexBuffer = {
				m_drawItems[i].m_staticMesh->m_indexBuffer, nvrhi::Format::R32_UINT, 0
			};

			if (!m_drawItems[i].m_isSkinned)
			{
				graphicsState.pipeline = m_Pipeline;
			}
			else
			{
				graphicsState.pipeline = m_SkinnedPipeline;
			}

			//update the pipeline, bindings, and other state.
			m_CommandList->setGraphicsState(graphicsState);

			// Draw the model.
			nvrhi::DrawArguments args;
			args.vertexCount = m_drawItems[i].m_staticMesh->m_totalIndices;
			args.instanceCount = 1;
			//args.startVertexLocation = m_drawItems[i].mesh->vertexOffset + m_drawItems[i].meshGeometry->vertexOffsetInMesh;
			//args.startIndexLocation = m_drawItems[i].mesh->indexOffset + m_drawItems[i].meshGeometry->indexOffsetInMesh;
			args.startVertexLocation = 0;
			args.startIndexLocation = 0;
			m_CommandList->drawIndexed(args);	
		}

		//draw terrain
		nvrhi::GraphicsState terrainGraphicsState;
		terrainGraphicsState.pipeline = m_terrainPipeline;
		terrainGraphicsState.framebuffer = inViewportClient->getFramebuffer();
		terrainGraphicsState.viewport.addViewportAndScissorRect(viewport);
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
			std::shared_ptr<TerrainComponent> terrainComponent = gameObjects[i]->getComponent<TerrainComponent>();
			if (transformComponent && terrainComponent)
			{
				if (terrainComponent->m_indexBufferHandle == nullptr || terrainComponent->m_vertexBufferHandle == nullptr)
				{
					createTerrainVertexBufferAndIndexBuffer(terrainComponent);

					terrainComponent->m_terrainPropertiesConstantBuffer.resize(terrainComponent->m_terrainCols * terrainComponent->m_terrainRows);
				}

				if (terrainComponent->getHeightTexture()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getHeightTexture(), m_commonRenderPass.get(), m_CommandList);
				}
				if (terrainComponent->getTerrainTexture1()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture1(), m_commonRenderPass.get(), m_CommandList);
				}
				if (terrainComponent->getTerrainTexture2()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture2(), m_commonRenderPass.get(), m_CommandList);
				}
				if (terrainComponent->getTerrainTexture3()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture3(), m_commonRenderPass.get(), m_CommandList);
				}
				if (terrainComponent->getTerrainTexture4()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getTerrainTexture4(), m_commonRenderPass.get(), m_CommandList);
				}
				if (terrainComponent->getBlendTexture()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getBlendTexture(), m_commonRenderPass.get(), m_CommandList);
				}
				if (!terrainComponent->m_terrainConstantBuffer)
				{
					terrainComponent->m_terrainConstantBuffer = GetDevice()->createBuffer(
						nvrhi::utils::CreateStaticConstantBufferDesc(
							sizeof(TerrainConstantBufferEntry), "TerrainConstantBufferEntry")
							.setInitialState(
							nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
							true));
				}

				if (terrainComponent->getBlendTexture()->m_isDirty)
				{
					//todo:update small region
					m_textureCache.FinalizeTexture(terrainComponent->getBlendTexture(), m_commonRenderPass.get(), m_CommandList);
					terrainComponent->getBlendTexture()->m_isDirty = false;
				}
				if (terrainComponent->getHeightTexture()->m_isDirty)
				{
					m_textureCache.FinalizeTexture(terrainComponent->getHeightTexture(), m_commonRenderPass.get(), m_CommandList);
					terrainComponent->getBlendTexture()->m_isDirty = false;
				}
			}
			else
			{
				continue;
			}
			TerrainConstantBufferEntry terrainConstants;
			terrainConstants.viewProjMatrix = viewProjMatrix;
			terrainConstants.worldMatrix = math::affineToHomogeneous(transformComponent->GetLocalToWorldTransformFloat());
			//get the global matrix to fill constant buffer		
			m_CommandList->writeBuffer(terrainComponent->m_terrainConstantBuffer, &terrainConstants, sizeof(TerrainConstantBufferEntry));
			
			math::float2 terrainSize = math::float2((float)terrainComponent->m_cols * (float)terrainComponent->m_tileSize, (float)terrainComponent->m_rows * (float)terrainComponent->m_tileSize);
			math::float2 terrainBeginXZ = math::float2(-(float)terrainComponent->m_terrainCols * terrainSize.x * 0.5f, -(float)terrainComponent->m_terrainRows * terrainSize.y * 0.5f);
			for (uint32_t i = 0; i < terrainComponent->m_terrainRows; ++i)
			{
				for (uint32_t j = 0; j < terrainComponent->m_terrainCols; ++j)
				{
					uint32_t index = i * terrainComponent->m_terrainCols + j;
					if (!terrainComponent->m_terrainPropertiesConstantBuffer[index])
					{
						terrainComponent->m_terrainPropertiesConstantBuffer[index] = GetDevice()->createBuffer(
							nvrhi::utils::CreateStaticConstantBufferDesc(
								sizeof(TerrainPropertiesBuffer), "TerrainPropertiesBuffer")
							.setInitialState(
								nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
									true));
					}
					math::float2 offsetXZ = math::float2(terrainBeginXZ.x + j * terrainSize.x + terrainSize.x / 2.0f, terrainBeginXZ.y + i * terrainSize.y + terrainSize.y / 2.0f);
					TerrainPropertiesBuffer propertiesBuffer;
					propertiesBuffer.m_heightScale = terrainComponent->m_heightScale;
					propertiesBuffer.m_beginXZ = terrainBeginXZ;
					propertiesBuffer.m_xzOffset = offsetXZ;
					m_CommandList->writeBuffer(terrainComponent->m_terrainPropertiesConstantBuffer[index], &propertiesBuffer, sizeof(TerrainPropertiesBuffer));
					//draw terrain
					nvrhi::BindingSetHandle terrainBindingSet;
					nvrhi::BindingSetDesc desc;
					//nvrhi::BindingSetHandle bindingSet;
					desc.bindings = {
							nvrhi::BindingSetItem::ConstantBuffer(0, terrainComponent->m_terrainConstantBuffer),
							nvrhi::BindingSetItem::ConstantBuffer(1, terrainComponent->m_terrainPropertiesConstantBuffer[index]),
							nvrhi::BindingSetItem::Texture_SRV(0, terrainComponent->getHeightTexture()->m_texture),
							nvrhi::BindingSetItem::Texture_SRV(1, terrainComponent->getBlendTexture()->m_texture),
							nvrhi::BindingSetItem::Texture_SRV(2, terrainComponent->getTerrainTexture1()->m_texture),
							nvrhi::BindingSetItem::Texture_SRV(3, terrainComponent->getTerrainTexture2()->m_texture),
							nvrhi::BindingSetItem::Texture_SRV(4, terrainComponent->getTerrainTexture3()->m_texture),
							nvrhi::BindingSetItem::Texture_SRV(5, terrainComponent->getTerrainTexture4()->m_texture),
							nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
					};
					terrainBindingSet = GetDevice()->createBindingSet(desc, m_terrainBindingLayout);

					terrainGraphicsState.bindings = { terrainBindingSet };

					terrainGraphicsState.vertexBuffers = {
						{terrainComponent->m_vertexBufferHandle, 0, 0},
					};

					terrainGraphicsState.indexBuffer = {
						terrainComponent->m_indexBufferHandle, nvrhi::Format::R32_UINT, 0
					};

					terrainGraphicsState.setPipeline(m_terrainPipeline);
					m_CommandList->setGraphicsState(terrainGraphicsState);

					// Draw the model.
					nvrhi::DrawArguments args;
					args.vertexCount = terrainComponent->m_indexData.size();
					args.instanceCount = 1;
					args.startVertexLocation = 0;
					args.startIndexLocation = 0;
					m_CommandList->drawIndexed(args);
				}
			}
		}

		//draw water
		nvrhi::GraphicsState waterGraphicsState;
		waterGraphicsState.pipeline = m_waterPipeline;
		waterGraphicsState.framebuffer = inViewportClient->getFramebuffer();
		waterGraphicsState.viewport.addViewportAndScissorRect(viewport);
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
			std::shared_ptr<WaterComponent> waterComponent = gameObjects[i]->getComponent<WaterComponent>();
			if (transformComponent && waterComponent)
			{
				if (waterComponent->m_indexBufferHandle == nullptr || waterComponent->m_vertexBufferHandle == nullptr)
				{
					createWaterVertexBufferAndIndexBuffer(waterComponent);
				}

				if (waterComponent->getDirTexture()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(waterComponent->getDirTexture(), m_commonRenderPass.get(), m_CommandList);
				}
				if (waterComponent->getColorTexture()->m_texture == nullptr)
				{
					m_textureCache.FinalizeTexture(waterComponent->getColorTexture(), m_commonRenderPass.get(), m_CommandList);
				}

				if (!waterComponent->m_waterConstantBuffer)
				{
					waterComponent->m_waterConstantBuffer = GetDevice()->createBuffer(
						nvrhi::utils::CreateStaticConstantBufferDesc(
							sizeof(WaterConstantBufferEntry), " WaterConstantBufferEntry")
						.setInitialState(
							nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
								true));
				}
				if (waterComponent->m_waterPropertiesConstantBuffer.empty())
				{
					waterComponent->m_waterPropertiesConstantBuffer.resize(waterComponent->m_waterCols * waterComponent->m_waterRows);
				}
			}
			else
			{
				continue;
			}
			WaterConstantBufferEntry waterConstants;
			waterConstants.viewProjMatrix = viewProjMatrix;
			waterConstants.worldMatrix = math::affineToHomogeneous(transformComponent->GetLocalToWorldTransformFloat());
			waterConstants.time = m_wallClockTime;
			//get the global matrix to fill constant buffer		
			m_CommandList->writeBuffer(waterComponent->m_waterConstantBuffer, &waterConstants, sizeof(WaterConstantBufferEntry));

			math::float2 waterSize = math::float2((float)waterComponent->m_rows * (float)waterComponent->m_tileSize, (float)waterComponent->m_cols * (float)waterComponent->m_tileSize);
			math::float2 waterBeginXZ = math::float2(-(float)waterComponent->m_waterRows * waterSize.x * 0.5f, -(float)waterComponent->m_waterCols * waterSize.y * 0.5f);
			for (uint32_t i = 0; i < waterComponent->m_waterRows; ++i)
			{
				for (uint32_t j = 0; j < waterComponent->m_waterCols; ++j)
				{
					uint32_t index = i * waterComponent->m_waterCols + j;
					if (!waterComponent->m_waterPropertiesConstantBuffer[index])
					{
						waterComponent->m_waterPropertiesConstantBuffer[index] = GetDevice()->createBuffer(
							nvrhi::utils::CreateStaticConstantBufferDesc(
								sizeof(WaterPropertiesBuffer), "WaterPropertiesBuffer")
							.setInitialState(
								nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
									true));
					}

					int32_t mixedColorImageWidth = waterComponent->getMixedColorTexture()->m_width;
					int32_t mixedColorImageHeight = waterComponent->getMixedColorTexture()->m_height;
					int32_t waterImagePx = (i / (float)waterComponent->m_waterRows) * mixedColorImageWidth;
					int32_t waterImagePy = ((waterComponent->m_waterCols - j - 1) / (float)waterComponent->m_waterCols) * mixedColorImageHeight;
					math::float3 mixedColor = waterComponent->getMixedColorTexture()->getColor(waterImagePx, waterImagePy);
					if (mixedColor.x >= 30.0f)
					{
						math::float2 offsetXZ = math::float2(waterBeginXZ.x + j * waterSize.x, waterBeginXZ.y + i * waterSize.y);
						WaterPropertiesBuffer propertiesBuffer;
						propertiesBuffer.m_heightScale = 10.0f;
						propertiesBuffer.m_beginXZ = waterBeginXZ;
						propertiesBuffer.m_xzOffset = offsetXZ;
						m_CommandList->writeBuffer(waterComponent->m_waterPropertiesConstantBuffer[index], &propertiesBuffer, sizeof(WaterPropertiesBuffer));
						//draw water
						nvrhi::BindingSetHandle waterBindingSet;
						nvrhi::BindingSetDesc desc;
						//nvrhi::BindingSetHandle bindingSet;
						desc.bindings = {
								nvrhi::BindingSetItem::ConstantBuffer(0, waterComponent->m_waterConstantBuffer),
								nvrhi::BindingSetItem::ConstantBuffer(1, waterComponent->m_waterPropertiesConstantBuffer[index]),
								nvrhi::BindingSetItem::Texture_SRV(0, waterComponent->getColorTexture()->m_texture),
								nvrhi::BindingSetItem::Texture_SRV(1, waterComponent->getDirTexture()->m_texture),
								nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
						};
						waterBindingSet = GetDevice()->createBindingSet(desc, m_waterBindingLayout);

						waterGraphicsState.bindings = { waterBindingSet };

						waterGraphicsState.vertexBuffers = {
							{waterComponent->m_vertexBufferHandle, 0, 0},
						};

						waterGraphicsState.indexBuffer = {
							waterComponent->m_indexBufferHandle, nvrhi::Format::R32_UINT, 0
						};

						waterGraphicsState.setPipeline(m_waterPipeline);
						m_CommandList->setGraphicsState(waterGraphicsState);

						// Draw the model.
						nvrhi::DrawArguments args;
						args.vertexCount = waterComponent->m_indexData.size();
						args.instanceCount = 1;
						args.startVertexLocation = 0;
						args.startIndexLocation = 0;
						m_CommandList->drawIndexed(args);
					}		
				}
			}
		}

		//draw skybox
		nvrhi::GraphicsState skyBoxGraphicsState;
		skyBoxGraphicsState.pipeline = m_skyBoxPipeline;
		skyBoxGraphicsState.framebuffer = inViewportClient->getFramebuffer();
		skyBoxGraphicsState.viewport.addViewportAndScissorRect(viewport);
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<CubeComponent> cubeComponent = gameObjects[i]->getComponent<CubeComponent>();
			if (cubeComponent)
			{
				if (cubeComponent->m_textureHandle == nullptr || cubeComponent->isDirty())
				{
					cubeComponent->setDirty(false);

					std::vector<std::shared_ptr<GTexture>> skyBoxTextures = {
						cubeComponent->getFrontTexture(),
						cubeComponent->getBackTexture(),
						cubeComponent->getTopTexture(),
						cubeComponent->getBottomTexture(),
						cubeComponent->getRightTexture(),
						cubeComponent->getLeftTexture(),
					};
					nvrhi::TextureHandle resultHandle = m_textureCache.FinalizeCubeMapTexture(skyBoxTextures, m_commonRenderPass.get(), m_CommandList);
					if(resultHandle != nullptr)
						cubeComponent->m_textureHandle = resultHandle;
				}
				//draw sky box
				nvrhi::BindingSetHandle skyBoxBindingSet;
				nvrhi::BindingSetDesc desc;
				//nvrhi::BindingSetHandle bindingSet;
				desc.bindings = {
						nvrhi::BindingSetItem::ConstantBuffer(0, m_skyBoxConstantBuffer),
						nvrhi::BindingSetItem::Texture_SRV(0, cubeComponent->m_textureHandle),
						nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
				};
				m_skyBoxBindingSet = GetDevice()->createBindingSet(desc, m_skyBoxBindingLayout);

				skyBoxGraphicsState.bindings = { m_skyBoxBindingSet };

				skyBoxGraphicsState.vertexBuffers = {
					//{cameraComponent->m_debugCameraFrustumVertexBuffer, 0, 0}
					{m_cube.m_vertexBuffer, 0, 0}
				};

				skyBoxGraphicsState.indexBuffer = {
					//cameraComponent->m_debugCameraFrustumIndexBuffer, nvrhi::Format::R32_UINT, 0
					m_cube.m_indexBuffer, nvrhi::Format::R32_UINT, 0
				};

				//math::float4x4 view = math::float4x4::zero();
				//for (int32_t row = 0; row < 3; ++row)
				//{
				//	for (int32_t col = 0; col < 3; ++col)
				//	{
				//		view[row][col] = inViewportClient->getWorldToViewMatrix()[row][col];
				//	}
				//}
				
				SkyBoxConstantBufferEntry modelConstants;
				modelConstants.viewProjMatrix = inViewportClient->getWorldToViewNoTranslationMatrix() * inViewportClient->getPespectiveMatrix();
				modelConstants.worldMatrix = math::float4x4::identity();;
				modelConstants.camWorldPos = inViewportClient->getCamPos();
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_skyBoxConstantBuffer, &modelConstants, sizeof(modelConstants));

				//CameraPropertiesBuffer propertiesBuffer;
				//propertiesBuffer.color = math::float3(0.73f, 0.90f, 0.44f);
				//m_CommandList->writeBuffer(m_cameraPropertiesConstantBuffers[i], &propertiesBuffer, sizeof(propertiesBuffer));

				skyBoxGraphicsState.setPipeline(m_skyBoxPipeline);
				m_CommandList->setGraphicsState(skyBoxGraphicsState);

				//draw the model
				nvrhi::DrawArguments args;
				args.vertexCount = 36;
				args.instanceCount = 1;
				args.startVertexLocation = 0;
				args.startIndexLocation = 0;
				m_CommandList->drawIndexed(args);
				break;
			}
		}

		//------draw game ui------
		std::vector<std::shared_ptr<UIDrawInfo>> uiDrawInfos;
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<UITransformComponent> transformComponent = gameObjects[i]->getComponent<UITransformComponent>();
			std::vector<std::shared_ptr<Component>> uiComponent = gameObjects[i]->getComponentParentClassIs<UIComponent>();

			for (size_t j = 0; j < uiComponent.size(); ++j)
			{
				std::shared_ptr<UIComponent> currentUIComponent = std::static_pointer_cast<UIComponent>(uiComponent[j]);

				if (currentUIComponent->GetType() == typeof(TextComponent))
				{
					//update atlas
					std::shared_ptr<GFont> font = std::static_pointer_cast<TextComponent>(currentUIComponent)->getFont();
					if (font->m_atlas->isNeedToUpdateAtlas())
					{
						updateAtlas(font->m_atlas);
					}
				}

				std::shared_ptr<UIDrawInfo> drawInfo = currentUIComponent->generateUIDrawInformation();
				if (transformComponent && currentUIComponent)
				{
					uiDrawInfos.push_back(drawInfo);
					//createDebugCameraFrustum(cameraComponent, transformComponent, inViewportClient);
				}
			}
		}

		//sort ui draw infos
		std::sort(uiDrawInfos.begin(), uiDrawInfos.end(), [](const std::shared_ptr<UIDrawInfo>& lhs, const std::shared_ptr<UIDrawInfo>& rhs) {
			return lhs->m_zorder > rhs->m_zorder;
		});

		nvrhi::GraphicsState gameUIGraphicsState;
		gameUIGraphicsState.pipeline = m_gameUIPipeline;
		gameUIGraphicsState.framebuffer = inViewportClient->getFramebuffer();

		gameUIGraphicsState.viewport.addViewportAndScissorRect(viewport);

		for (int32_t i = 0; i < uiDrawInfos.size(); ++i)
		{
			std::shared_ptr<UIDrawInfo> drawInfo = uiDrawInfos[i];

			if (drawInfo->m_drawType == UIDrawType::Font)
			{
				gameUIGraphicsState.pipeline = m_gameUIFontPipeline;
			}
			else
			{
				gameUIGraphicsState.pipeline = m_gameUIPipeline;
			}

			createUIVertexBufferAndIndexBuffer(drawInfo);

			if (drawInfo->m_texture->m_texture == nullptr)
			{
				m_textureCache.FinalizeTexture(drawInfo->m_texture, m_commonRenderPass.get(), m_CommandList);
			}

			//draw
			nvrhi::BindingSetHandle uiBindingSet;
			nvrhi::BindingSetDesc desc;
			//nvrhi::BindingSetHandle bindingSet;
			desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_gameUIConstantBuffer[i]),
					nvrhi::BindingSetItem::ConstantBuffer(1, m_gameUIPropertiesConstantBuffers[i]),
					nvrhi::BindingSetItem::Texture_SRV(0, drawInfo->m_texture->m_texture),
					nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler),
			};
			uiBindingSet = GetDevice()->createBindingSet(desc, m_gameUIBindingLayout);

			gameUIGraphicsState.bindings = { uiBindingSet };

			gameUIGraphicsState.vertexBuffers = {
				{ drawInfo->m_uiVertexHandle, 0, offsetof(GameUIVertex, m_uv)},
				{ drawInfo->m_uiVertexHandle, 1, offsetof(GameUIVertex, m_position)},
				{ drawInfo->m_uiVertexHandle, 2, offsetof(GameUIVertex, m_color)},
			};

			gameUIGraphicsState.indexBuffer = {
				drawInfo->m_uiIndexHandle, nvrhi::Format::R32_UINT, 0
			};

			GameUIBufferEntry modelConstants;
			modelConstants.viewProjMatrix = viewProjMatrix;
			modelConstants.worldMatrix = math::float4x4::identity();
			modelConstants.camWorldPos = inViewportClient->getCamPos();
			//get the global matrix to fill constant buffer		
			m_CommandList->writeBuffer(m_gameUIConstantBuffer[i], &modelConstants, sizeof(modelConstants));

			GameUIPropertiesBuffer propertiesBuffer;
			propertiesBuffer.color = math::float3(1.0f, 1.0f, 1.0f);
			m_CommandList->writeBuffer(m_gameUIPropertiesConstantBuffers[i], &propertiesBuffer, sizeof(propertiesBuffer));

			//gameUIGraphicsState.setPipeline(m_gameUIPipeline);
			m_CommandList->setGraphicsState(gameUIGraphicsState);

			//draw the model
			nvrhi::DrawArguments args;
			args.vertexCount = drawInfo->m_uiIndices.size();
			args.instanceCount = 1;
			args.startVertexLocation = 0;
			args.startIndexLocation = 0;
			m_CommandList->drawIndexed(args);
		}
		//------draw game ui------

		//-----draw game debug info------
		if (inViewportClient->getSelectedItems())
		{
			std::shared_ptr<UIComponent> uiComponent = inViewportClient->getSelectedItems()->getComponent<UIComponent>();
			if (uiComponent)
			{
				std::shared_ptr<UIDebugInfo> debugInfo = uiComponent->generateUIDebugInfomartion(inViewportClient->getDebugLineWidth());
				if (debugInfo)
				{
					nvrhi::GraphicsState gameUIDebugGraphicsState;
					gameUIDebugGraphicsState.pipeline = m_gameUIDebugPipeline;
					gameUIDebugGraphicsState.framebuffer = inViewportClient->getFramebuffer();
					gameUIDebugGraphicsState.viewport.addViewportAndScissorRect(viewport);
		
					createUIDebugVertexBufferAndIndexBuffer(debugInfo);
		
					//draw
					nvrhi::BindingSetHandle uiBindingSet;
					nvrhi::BindingSetDesc desc;
					//nvrhi::BindingSetHandle bindingSet;
					desc.bindings = {
							nvrhi::BindingSetItem::ConstantBuffer(0, m_gameUIDebugConstantBuffer[0]),
							nvrhi::BindingSetItem::ConstantBuffer(1, m_gameUIDebugPropertiesConstantBuffers[0]),
					};
					uiBindingSet = GetDevice()->createBindingSet(desc, m_gameUIDebugBindingLayout);
		
					gameUIDebugGraphicsState.bindings = { uiBindingSet };
		
					gameUIDebugGraphicsState.vertexBuffers = {
						{debugInfo->m_uiDebugVertexHandle, 0, 0,}
					};
		
					gameUIDebugGraphicsState.indexBuffer = {
						debugInfo->m_uiDebugIndexHandle, nvrhi::Format::R32_UINT, 0
					};
		
					GameUIDebugBufferEntry modelConstants;
					modelConstants.viewProjMatrix = viewProjMatrix;
					modelConstants.worldMatrix = math::float4x4::identity();
					modelConstants.camWorldPos = inViewportClient->getCamPos();
					//get the global matrix to fill constant buffer		
					m_CommandList->writeBuffer(m_gameUIDebugConstantBuffer[0], &modelConstants, sizeof(modelConstants));
		
					GameUIDebugPropertiesBuffer propertiesBuffer;
					propertiesBuffer.color = math::float3(1.0f, 1.0f, 1.0f);
					m_CommandList->writeBuffer(m_gameUIDebugPropertiesConstantBuffers[0], &propertiesBuffer, sizeof(propertiesBuffer));
		
					//gameUIGraphicsState.setPipeline(m_gameUIPipeline);
					m_CommandList->setGraphicsState(gameUIDebugGraphicsState);
		
					//draw the model
					nvrhi::DrawArguments args;
					args.vertexCount = debugInfo->m_uiDebugIndices.size();
					args.instanceCount = 1;
					args.startVertexLocation = 0;
					args.startIndexLocation = 0;
					m_CommandList->drawIndexed(args);
				}
			}	
		}
		//-----draw game debug info------

		nvrhi::GraphicsState gridGraphicsState;
		gridGraphicsState.pipeline = m_gridPipeline;
		gridGraphicsState.framebuffer = inViewportClient->getFramebuffer();
		gridGraphicsState.viewport.addViewportAndScissorRect(viewport);

		GridProperties gridProperties;
		m_CommandList->writeBuffer(m_gridPropertiesConstantBuffer, &gridProperties, sizeof(GridProperties));

		//draw grid
		if (!m_gridBindingSet)
		{
			nvrhi::BindingSetDesc desc;
			//nvrhi::BindingSetHandle bindingSet;
			desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_gridConstantBuffer),
					nvrhi::BindingSetItem::ConstantBuffer(1, m_gridPropertiesConstantBuffer),
			};
			m_gridBindingSet = GetDevice()->createBindingSet(desc, m_gridBindingLayout);
		}

		gridGraphicsState.bindings = { m_gridBindingSet };

		gridGraphicsState.vertexBuffers = {
			{m_gridVertexBuffer, 0, offsetof(GridVertex, position)},
			{m_gridVertexBuffer, 1, offsetof(GridVertex, uv)}
		};

		GridConstantBufferEntry modelConstants;
		modelConstants.viewProjMatrix = viewProjMatrix;
		modelConstants.worldMatrix = math::float4x4::identity();
		modelConstants.camWorldPos = inViewportClient->getCamPos();
		//get the global matrix to fill constant buffer		
		m_CommandList->writeBuffer(m_gridConstantBuffer, &modelConstants, sizeof(modelConstants));

		gridGraphicsState.setPipeline(m_gridPipeline);
		m_CommandList->setGraphicsState(gridGraphicsState);

		// Draw the model.
		nvrhi::DrawArguments args;
		args.vertexCount = m_gridVertices.size();
		m_CommandList->draw(args);

		m_CommandList->clearDepthStencilTexture(inViewportClient->getDepthTarget(), nvrhi::AllSubresources, true, 1.0f, true, 0);
		nvrhi::GraphicsState gizmosGraphicsState;
		gizmosGraphicsState.pipeline = m_gizmosPipeline;
		gizmosGraphicsState.framebuffer = inViewportClient->getFramebuffer();

		gizmosGraphicsState.viewport.addViewportAndScissorRect(viewport);

		std::vector<uint32_t> gizmosRenderSort = inViewportClient->getGizmosRenderSort();
		if (inViewportClient->gizmosIsVisible())
		{
			//draw gizmos
			std::vector<std::shared_ptr<GStaticMesh>>& gizmosStaticMesh = inViewportClient->getGizmos();
			uint32_t i = 0;
			for (uint32_t index = 0; index < gizmosRenderSort.size(); ++index)
			{			
				auto& item = gizmosStaticMesh[gizmosRenderSort[index]];
				if (item->m_vertexBuffer == nullptr)
				{
					createVertexBufferAndIndexBuffer(*item);
				}

				//draw
				nvrhi::BindingSetHandle gizmosBindingSet;
				nvrhi::BindingSetDesc desc;
				//nvrhi::BindingSetHandle bindingSet;
				desc.bindings = {
						nvrhi::BindingSetItem::ConstantBuffer(0, m_gizmosConstantBuffer),
						nvrhi::BindingSetItem::ConstantBuffer(1, m_gizmosPropertiesConstantBuffers[i]),
				};
				gizmosBindingSet = GetDevice()->createBindingSet(desc, m_gizmosBindingLayout);

				gizmosGraphicsState.bindings = { gizmosBindingSet };

				gizmosGraphicsState.vertexBuffers = {
					{item->m_vertexBuffer, 0, item->getVertexBufferRange(GVertexAttribute::Position).byteOffset},
					{item->m_vertexBuffer, 1, item->getVertexBufferRange(GVertexAttribute::TexCoord1).byteOffset},
					{item->m_vertexBuffer, 2, item->getVertexBufferRange(GVertexAttribute::Normal).byteOffset},
					{item->m_vertexBuffer, 3, item->getVertexBufferRange(GVertexAttribute::Tangent).byteOffset},
				};

				gizmosGraphicsState.indexBuffer = {
					item->m_indexBuffer, nvrhi::Format::R32_UINT, 0
				};

				GizmosConstantBufferEntry modelConstants;
				modelConstants.viewProjMatrix = viewProjMatrix;
				math::float3 translation;
				math::float3 scaling;
				math::quat rotation;
				std::shared_ptr<TransformComponent> transform = inViewportClient->getSelectedItems()->getComponent<TransformComponent>();
				math::affine3 transformAffine = transform->GetLocalToWorldTransformFloat();
				math::decomposeAffine(transformAffine, &translation, &rotation, &scaling);
				//translation = math::float3(transform->getTranslation());
				//scaling = math::float3(transform->getScaling());
				//rotation = math::quat(transform->getRotation());
				math::affine3 noScalingAffine;//gizmos 不需要缩放
				scaling = math::float3(inViewportClient->getScreenScaleCompensation(translation)) * 100.0f;
				//GuGu_LOGD("%f", scaling.x);
				if (inViewportClient->getCurrentGizmosType() == ViewportClient::Gizmos::Move)
				{
					noScalingAffine = math::scaling(scaling) * math::translation(translation);
				}
				else if (inViewportClient->getCurrentGizmosType() == ViewportClient::Gizmos::Rotation)
				{
					noScalingAffine = math::scaling(scaling) * math::translation(translation);
				}		
				else if (inViewportClient->getCurrentGizmosType() == ViewportClient::Gizmos::Scale)
				{
					noScalingAffine = math::scaling(scaling) * math::affine3(transform->getRotationQuat().toAffine()) * math::translation(translation);
				}
				modelConstants.worldMatrix = math::float4x4(math::affineToHomogeneous(noScalingAffine));
				modelConstants.camWorldPos = inViewportClient->getCamPos();
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_gizmosConstantBuffer, &modelConstants, sizeof(modelConstants));

				GizmosPropertiesBuffer propertiesBuffer;
				propertiesBuffer.color = inViewportClient->getGizmosColor(gizmosRenderSort[index]).xyz();
				m_CommandList->writeBuffer(m_gizmosPropertiesConstantBuffers[i], &propertiesBuffer, sizeof(propertiesBuffer));

				gizmosGraphicsState.setPipeline(m_gizmosPipeline);
				m_CommandList->setGraphicsState(gizmosGraphicsState);

				// Draw the model.
				nvrhi::DrawArguments args;
				args.vertexCount = item->m_indexData.size();
				args.instanceCount = 1;
				args.startVertexLocation = 0;
				args.startIndexLocation = 0;
				m_CommandList->drawIndexed(args);

				++i;
			}

			//draw pick pos
			{
				const auto& item = std::static_pointer_cast<GStaticMesh>(AssetManager::getAssetManager().getAssetData("content/defaultCube.json").m_loadedResource);

				//todo:draw pick pos
			}
		}	

		nvrhi::GraphicsState cameraGraphicsState;
		cameraGraphicsState.pipeline = m_cameraPipeline;
		cameraGraphicsState.framebuffer = inViewportClient->getFramebuffer();

		cameraGraphicsState.viewport.addViewportAndScissorRect(viewport);

		//draw debug camera
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
			std::shared_ptr<CameraComponent> cameraComponent = gameObjects[i]->getComponent<CameraComponent>();
			if (transformComponent && cameraComponent) //debug
			{
				//if(!cameraComponent->m_debugCameraFrustumVertexBuffer)
				createDebugCameraFrustum(cameraComponent, transformComponent, inViewportClient);

				//draw
				nvrhi::BindingSetHandle cameraBindingSet;
				nvrhi::BindingSetDesc desc;
				//nvrhi::BindingSetHandle bindingSet;
				desc.bindings = {
						nvrhi::BindingSetItem::ConstantBuffer(0, m_cameraConstantBuffer[i]),
						nvrhi::BindingSetItem::ConstantBuffer(1, m_cameraPropertiesConstantBuffers[i]),
				};
				cameraBindingSet = GetDevice()->createBindingSet(desc, m_cameraBindingLayout);

				cameraGraphicsState.bindings = { cameraBindingSet };

				cameraGraphicsState.vertexBuffers = {
					{cameraComponent->m_debugCameraFrustumVertexBuffer, 0, 0}
				};

				cameraGraphicsState.indexBuffer = {
					cameraComponent->m_debugCameraFrustumIndexBuffer, nvrhi::Format::R32_UINT, 0
				};

				CameraBufferEntry modelConstants;
				modelConstants.viewProjMatrix = viewProjMatrix;
				modelConstants.worldMatrix = math::float4x4::identity();;
				modelConstants.camWorldPos = inViewportClient->getCamPos();
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_cameraConstantBuffer[i], &modelConstants, sizeof(modelConstants));

				CameraPropertiesBuffer propertiesBuffer;
				propertiesBuffer.color = math::float3(0.73f, 0.90f, 0.44f);
				m_CommandList->writeBuffer(m_cameraPropertiesConstantBuffers[i], &propertiesBuffer, sizeof(propertiesBuffer));

				cameraGraphicsState.setPipeline(m_cameraPipeline);
				m_CommandList->setGraphicsState(cameraGraphicsState);

				// Draw the model.
				nvrhi::DrawArguments args;
				args.vertexCount = 24;
				args.instanceCount = 1;
				args.startVertexLocation = 0;
				args.startIndexLocation = 0;
				m_CommandList->drawIndexed(args);
			}
		}

		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);
	}
	void Demo::RenderView(nvrhi::GraphicsState& graphicsState, nvrhi::GraphicsState& gridGraphicsState, math::float4x4 viewProjMatrix)
	{
		m_drawItems.clear();
		//render view

		//traverse tree
		SceneGraphWalker walker(m_sceneGraph->GetRootNode().get());
		
		uint32_t index = 0;
		while (walker)
		{
			auto meshInstance = dynamic_cast<MeshInstance*>(walker->GetLeaf().get());
		
			if (meshInstance)
			{
				const MeshInfo* mesh = meshInstance->getMesh().get();
		
				for (const auto& geometry : mesh->geometries)
				{
					DrawItem drawItem;
					drawItem.mesh = mesh;
					drawItem.bufferGroup = mesh->buffers.get();
					drawItem.meshGeometry = geometry.get();
					drawItem.m_worldMatrix = m_ConstantBuffers[index];
					drawItem.m_pbrMaterial = m_PbrMaterialBuffers[index];
					drawItem.m_isSkinned = false;
					m_drawItems.push_back(drawItem);								
				}
		
				//get the world matrix
				SceneGraphNode* node = meshInstance->GetNode();
				ConstantBufferEntry modelConstants;
				modelConstants.viewProjMatrix = viewProjMatrix;
				modelConstants.worldMatrix = math::affineToHomogeneous(node->GetLocalToWorldTransformFloat());
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_ConstantBuffers[index], &modelConstants, sizeof(modelConstants));
		
				PbrMaterial pbrMaterial;
				pbrMaterial.albedo = m_uiData->color;
				pbrMaterial.metallic = m_uiData->metallic;;
				pbrMaterial.roughness = m_uiData->roughness;
				pbrMaterial.ao = 0.0f;
				m_CommandList->writeBuffer(m_PbrMaterialBuffers[index], &pbrMaterial, sizeof(pbrMaterial));
				++index;
			}
			
			auto skinnedMeshInstance = dynamic_cast<SkinnedMeshInstance*>(walker->GetLeaf().get());
			if (skinnedMeshInstance)
			{
				const MeshInfo* mesh = skinnedMeshInstance->getMesh().get();
		
				for (const auto& geometry : mesh->geometries)
				{
					DrawItem drawItem;
					drawItem.mesh = mesh;
					drawItem.bufferGroup = mesh->skinPrototype->buffers.get();
					drawItem.meshGeometry = geometry.get();
					drawItem.m_worldMatrix = m_ConstantBuffers[index];
					drawItem.m_pbrMaterial = m_PbrMaterialBuffers[index];
					drawItem.m_isSkinned = true;
					drawItem.m_skinnedMatrix = skinnedMeshInstance->jointBuffer;
					m_drawItems.push_back(drawItem);
				}
		
				//get the world matrix
				SceneGraphNode* node = skinnedMeshInstance->GetNode();
				ConstantBufferEntry modelConstants;
				modelConstants.viewProjMatrix = math::affineToHomogeneous(node->GetLocalToWorldTransformFloat()) * viewProjMatrix;
				modelConstants.worldMatrix = math::float4x4::identity();
				//get the global matrix to fill constant buffer		
				m_CommandList->writeBuffer(m_ConstantBuffers[index], &modelConstants, sizeof(modelConstants));
				PbrMaterial pbrMaterial;
				pbrMaterial.albedo = m_uiData->color;
				pbrMaterial.metallic = m_uiData->metallic;;
				pbrMaterial.roughness = m_uiData->roughness;
				pbrMaterial.ao = 0.0f;
				m_CommandList->writeBuffer(m_PbrMaterialBuffers[index], &pbrMaterial, sizeof(pbrMaterial));
				++index;
			}
		
			walker.Next(true);
		}		
		
		for (size_t i = 0; i < m_drawItems.size(); ++i)
		{
			nvrhi::BindingSetDesc desc;
		
			nvrhi::BindingSetHandle bindingSet;
			if (!m_drawItems[i].m_isSkinned)
			{			
				desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_drawItems[i].m_worldMatrix),
					nvrhi::BindingSetItem::ConstantBuffer(2, m_drawItems[i].m_pbrMaterial),
					nvrhi::BindingSetItem::ConstantBuffer(3, m_LightBuffers),
					nvrhi::BindingSetItem::ConstantBuffer(4, m_PassBuffers),
					nvrhi::BindingSetItem::Texture_SRV(0, m_commonRenderPass->m_whiteTexture),
					nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
				};
				bindingSet = GetDevice()->createBindingSet(desc, m_BindingLayout);
			}
			else
			{
				desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_drawItems[i].m_worldMatrix),
					nvrhi::BindingSetItem::ConstantBuffer(1, m_drawItems[i].m_skinnedMatrix),
					nvrhi::BindingSetItem::ConstantBuffer(2, m_drawItems[i].m_pbrMaterial),
					nvrhi::BindingSetItem::ConstantBuffer(3, m_LightBuffers),
					nvrhi::BindingSetItem::ConstantBuffer(4, m_PassBuffers),
					nvrhi::BindingSetItem::Texture_SRV(0, m_commonRenderPass->m_whiteTexture),
					nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
				};
				bindingSet = GetDevice()->createBindingSet(desc, m_SkinnedBindingLayout);
			}
		
			graphicsState.bindings = { bindingSet };
			
			if(!m_drawItems[i].bufferGroup->vertexBuffer)
				continue;
		
			graphicsState.vertexBuffers = {
						{m_drawItems[i].bufferGroup->vertexBuffer, 0, m_drawItems[i].bufferGroup->getVertexBufferRange(VertexAttribute::Position).byteOffset},
						{m_drawItems[i].bufferGroup->vertexBuffer, 1, m_drawItems[i].bufferGroup->getVertexBufferRange(VertexAttribute::TexCoord1).byteOffset},
						{m_drawItems[i].bufferGroup->vertexBuffer, 2, m_drawItems[i].bufferGroup->getVertexBufferRange(VertexAttribute::Normal).byteOffset},
						{m_drawItems[i].bufferGroup->vertexBuffer, 3, m_drawItems[i].bufferGroup->getVertexBufferRange(VertexAttribute::Tangent).byteOffset},
						{m_drawItems[i].bufferGroup->vertexBuffer, 4, m_drawItems[i].bufferGroup->getVertexBufferRange(VertexAttribute::JointWeights).byteOffset},
						{m_drawItems[i].bufferGroup->vertexBuffer, 5, m_drawItems[i].bufferGroup->getVertexBufferRange(VertexAttribute::JointIndices).byteOffset}
			};
		
			graphicsState.indexBuffer = {
				m_drawItems[i].bufferGroup->indexBuffer, nvrhi::Format::R32_UINT, 0
			};
		
			if (!m_drawItems[i].m_isSkinned)
			{
				graphicsState.pipeline = m_Pipeline;
			}
			else
			{
				graphicsState.pipeline = m_SkinnedPipeline;
			}
			
			//update the pipeline, bindings, and other state.
			m_CommandList->setGraphicsState(graphicsState);
			
			// Draw the model.
			nvrhi::DrawArguments args;
			args.vertexCount = m_drawItems[i].meshGeometry->numIndices;
			args.instanceCount = 1;
			args.startVertexLocation = m_drawItems[i].mesh->vertexOffset + m_drawItems[i].meshGeometry->vertexOffsetInMesh;
			args.startIndexLocation = m_drawItems[i].mesh->indexOffset + m_drawItems[i].meshGeometry->indexOffsetInMesh;
			m_CommandList->drawIndexed(args);
		}

		m_CommandList->writeBuffer(m_gridPropertiesConstantBuffer, &m_uiData->m_gridProperties, sizeof(GridProperties));

		//draw grid
		if (!m_gridBindingSet)
		{
			nvrhi::BindingSetDesc desc;
			//nvrhi::BindingSetHandle bindingSet;
			desc.bindings = {
					nvrhi::BindingSetItem::ConstantBuffer(0, m_gridConstantBuffer),
					nvrhi::BindingSetItem::ConstantBuffer(1, m_gridPropertiesConstantBuffer),
			};
			m_gridBindingSet = GetDevice()->createBindingSet(desc, m_gridBindingLayout);
		}

		gridGraphicsState.bindings = { m_gridBindingSet };

		gridGraphicsState.vertexBuffers = {
			{m_gridVertexBuffer, 0, offsetof(GridVertex, position)},
			{m_gridVertexBuffer, 1, offsetof(GridVertex, uv)}
		};

		GridConstantBufferEntry modelConstants;
		modelConstants.viewProjMatrix = viewProjMatrix;
		modelConstants.worldMatrix = math::float4x4::identity();
		modelConstants.camWorldPos = m_uiData->camPos;
		//get the global matrix to fill constant buffer		
		m_CommandList->writeBuffer(m_gridConstantBuffer, &modelConstants, sizeof(modelConstants));

		gridGraphicsState.setPipeline(m_gridPipeline);
		m_CommandList->setGraphicsState(gridGraphicsState);	

		// Draw the model.
		nvrhi::DrawArguments args;
		args.vertexCount = m_gridVertices.size();
		m_CommandList->draw(args);
	}
	nvrhi::BufferHandle Demo::createGeometryBuffer(const GuGuUtf8Str& debugName, const void* data, uint64_t dataSize, bool isVertexBuffer)
	{
		nvrhi::BufferHandle bufferHandle;

		nvrhi::BufferDesc desc;
		desc.byteSize = dataSize;
		desc.isVertexBuffer = isVertexBuffer;
		desc.isIndexBuffer = !isVertexBuffer;
		desc.debugName = debugName;
		desc.initialState = nvrhi::ResourceStates::CopyDest;
		
		bufferHandle = GetDevice()->createBuffer(desc);

		if (data)
		{
			m_CommandList->beginTrackingBufferState(bufferHandle, nvrhi::ResourceStates::CopyDest);
			m_CommandList->writeBuffer(bufferHandle, data, dataSize);
			m_CommandList->setPermanentBufferState(bufferHandle, isVertexBuffer ? nvrhi::ResourceStates::VertexBuffer : nvrhi::ResourceStates::IndexBuffer);
		}

		return bufferHandle;
	}

	void Demo::initRenderTargetAndDepthTarget(ViewportClient& viewportClient, math::float2 viewportSize)
	{
		nvrhi::TextureDesc desc;
		desc.width = viewportSize.x;
		desc.height = viewportSize.y;
		desc.initialState = nvrhi::ResourceStates::RenderTarget;
		desc.isRenderTarget = true;
		desc.useClearValue = true;
		desc.clearValue = Color(0.f);
		desc.sampleCount = 1;
		desc.dimension = nvrhi::TextureDimension::Texture2D;
		desc.keepInitialState = true;
		desc.isTypeless = false;
		desc.isUAV = false;
		desc.mipLevels = 1;

		desc.format = nvrhi::Format::SRGBA8_UNORM;
		desc.debugName = "RenderTarget";
		nvrhi::TextureHandle renderTarget = GetDevice()->createTexture(desc);

		const nvrhi::Format depthFormats[] = {
			nvrhi::Format::D24S8,
			nvrhi::Format::D32S8,
			nvrhi::Format::D32,
			nvrhi::Format::D16 };

		const nvrhi::FormatSupport depthFeatures =
			nvrhi::FormatSupport::Texture |
			nvrhi::FormatSupport::DepthStencil |
			nvrhi::FormatSupport::ShaderLoad;

		desc.format = nvrhi::utils::ChooseFormat(GetDevice(), depthFeatures, depthFormats, std::size(depthFormats));
		desc.isTypeless = true;
		desc.initialState = nvrhi::ResourceStates::DepthWrite;
		desc.clearValue = Color(1.f);
		desc.debugName = "Depth";
		nvrhi::TextureHandle depthTarget = GetDevice()->createTexture(desc);

		nvrhi::FramebufferDesc fbDesc;
		fbDesc.addColorAttachment(renderTarget, nvrhi::TextureSubresourceSet(0, 1, 0, 1));
		fbDesc.setDepthAttachment(depthTarget, nvrhi::TextureSubresourceSet(0, 1, 0, 1));

		nvrhi::FramebufferHandle framebuffer = GetDevice()->createFramebuffer(fbDesc);
		viewportClient.setRenderTarget(renderTarget, depthTarget, framebuffer);
	}

	void Demo::getNodeChildrens(GuGuUtf8Str nodeName, std::vector<GuGuUtf8Str>& nodeChildrenNames)
	{
		std::shared_ptr<SceneGraphNode> findNode = m_sceneGraph->findNode(nodeName, m_sceneGraph->GetRootNode().get());//需要修复这个 find node 函数

		if (findNode)
		{
			SceneGraphNode* child;
			for (child = findNode->GetFirstChild(); child; child = child->GetNextSibling())
			{
				nodeChildrenNames.push_back(child->GetName());
			}
		}
	}

	void Demo::selectionChanged(GuGuUtf8Str item, SelectInfo::Type)
	{
		std::shared_ptr<SceneGraphNode> findNode = m_sceneGraph->findNode(item, m_sceneGraph->GetRootNode().get());//需要修复这个 find node 函数

		m_uiData->m_currentSelectItems = findNode;
	}
}