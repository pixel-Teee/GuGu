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

namespace GuGu {
	inline void AppendBufferRange(nvrhi::BufferRange& range, size_t size, uint64_t& currentBufferSize)
	{
		range.byteOffset = currentBufferSize;
		range.byteSize = size;
		currentBufferSize += range.byteSize;
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

		m_ConstantBuffers.resize(m_sceneGraph->GetMeshes().size());
		m_PbrMaterialBuffers.resize(m_sceneGraph->GetMeshes().size());
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
						.setFormat(nvrhi::Format::RGBA8_SNORM)
						.setOffset(0)
						.setBufferIndex(2)
						.setElementStride(sizeof(uint32_t)),
				nvrhi::VertexAttributeDesc()
						.setName("TANGENT")
						.setFormat(nvrhi::Format::RGBA8_SNORM)
						.setOffset(0)
						.setBufferIndex(3)
						.setElementStride(sizeof(uint32_t)),		
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

		TextureCache textureCache(GetDevice(), m_rootFileSystem);
		//todo:load texture
		GuGuUtf8Str textureFileName = u8"asset/fun.jpg";
		std::shared_ptr <LoadedTexture> texture = textureCache.LoadTextureFromFile(
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

		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);

		math::uint2 size = math::uint2(1280.0f, 960.0f);
		if (!m_renderTarget || math::any(m_renderTargetSize != size))
		{
			m_renderTarget = nullptr;

			m_renderTargetSize = size;

			initRenderTargetAndDepthTarget();
		}

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
	void Demo::renderLevel(const std::shared_ptr<Level> inLevel)
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
			m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, m_frameBuffer);
		}

		m_drawItems.clear();

		m_CommandList->open();
		
		nvrhi::utils::ClearColorAttachment(m_CommandList, m_frameBuffer, 0, Color(0.2f, 0.3f, 0.7f, 1.0f));
		m_CommandList->clearDepthStencilTexture(m_depthTarget, nvrhi::AllSubresources, true, 1.0f, true, 0);

		math::float3 cameraPos = math::float3(0.0f, 0.0f, m_uiData->camPos);
		math::float3 cameraDir = normalize(math::float3(0.0f, m_uiData->dir, 1.0f) - cameraPos);
		math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
		math::float3 cameraRight = normalize(cross(cameraUp, cameraDir));
		cameraUp = normalize(cross(cameraDir, cameraRight));

		math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, -cameraPos);

		//ConstantBufferEntry modelConstants;
		//math::affine3 viewMatrix =
		//math::yawPitchRoll(0.f, math::radians(-30.f), 0.f)
		//* math::translation(math::float3(0, 0, 2));
		math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(45.f),
			float(640) /
			float(400), 1.0f, 400.0f
		);
		math::float4x4 viewProjMatrix = math::affineToHomogeneous(worldToView) * projMatrix;
		//modelConstants.viewProjMatrix = viewProjMatrix;

		Pass pass;
		pass.camPos = cameraPos;
		m_CommandList->writeBuffer(m_PassBuffers, &pass, sizeof(pass));

		//draw level
		const std::vector<std::shared_ptr<GameObject>>& gameObjects = inLevel->getGameObjects();
		//暂时用cube代替绘制
		for (size_t i = 0; i < gameObjects.size(); ++i)
		{
			std::shared_ptr<TransformComponent> transformComponent = gameObjects[i]->getComponent<TransformComponent>();
			
			const math::affine3& worldMatrix = transformComponent->GetLocalToWorldTransformFloat();

			DrawItem drawItem;
			drawItem.mesh = m_meshInfo.get();
			drawItem.bufferGroup = m_meshInfo->buffers.get();
			drawItem.meshGeometry = m_meshInfo->geometries[0].get();
			drawItem.m_worldMatrix = m_cubeConstantBuffer;
			//drawItem.m_pbrMaterial = m_PbrMaterialBuffers[index];
			drawItem.m_isSkinned = false;
			m_drawItems.push_back(drawItem);

			//get the world matrix
			ConstantBufferEntry modelConstants;
			modelConstants.viewProjMatrix = viewProjMatrix;
			modelConstants.worldMatrix = math::affineToHomogeneous(worldMatrix);
			//get the global matrix to fill constant buffer		
			m_CommandList->writeBuffer(m_cubeConstantBuffer, &modelConstants, sizeof(modelConstants));

			//PbrMaterial pbrMaterial;
			//pbrMaterial.albedo = m_uiData->color;
			//pbrMaterial.metallic = m_uiData->metallic;;
			//pbrMaterial.roughness = m_uiData->roughness;
			//pbrMaterial.ao = 0.0f;
			//m_CommandList->writeBuffer(m_PbrMaterialBuffers[index], &pbrMaterial, sizeof(pbrMaterial));
			//++index;
		}

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
		graphicsState.framebuffer = m_frameBuffer;

		// Construct the viewport so that all viewports form a grid.
		const float width = float(1280.0f);
		const float height = float(720.0f);
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

			if (!m_drawItems[i].bufferGroup->vertexBuffer)
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

	void Demo::initRenderTargetAndDepthTarget()
	{
		nvrhi::TextureDesc desc;
		desc.width = m_renderTargetSize.x;
		desc.height = m_renderTargetSize.y;
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
		m_renderTarget = GetDevice()->createTexture(desc);

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
		m_depthTarget = GetDevice()->createTexture(desc);

		nvrhi::FramebufferDesc fbDesc;
		fbDesc.addColorAttachment(m_renderTarget, nvrhi::TextureSubresourceSet(0, 1, 0, 1));
		fbDesc.setDepthAttachment(m_depthTarget, nvrhi::TextureSubresourceSet(0, 1, 0, 1));

		m_frameBuffer = GetDevice()->createFramebuffer(fbDesc);
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