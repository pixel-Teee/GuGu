#include <pch.h>

#include "ModelImporter.h"

#include <Core/Model/StaticMesh.h>
#include <Core/AssetManager/AssetManager.h>

namespace GuGu {
	inline void AppendBufferRange(nvrhi::BufferRange& range, size_t size, uint64_t& currentBufferSize)
	{
		range.byteOffset = currentBufferSize;
		range.byteSize = size;
		currentBufferSize += range.byteSize;
	}

	math::float4x4 ConvertMatrixToMyMathMatrix(const aiMatrix4x4& from)
	{
		math::float4x4 to;
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}
	ModelImporter::ModelImporter()
	{
		//init
		
	}
	ModelImporter::~ModelImporter()
	{
	}
	nlohmann::json ModelImporter::loadModel(const GuGuUtf8Str& modelPhysicalFilePath, bool isSkeleton)
	{
		m_isSkeleton = isSkeleton;
		m_boneStartIndex = -1;
		//从文件的实际路径去加载模型
		Assimp::Importer import;
		
		const aiScene* scene = import.ReadFile(modelPhysicalFilePath.getStr(), aiProcess_Triangulate | aiProcess_FlipUVs);

		//加载模型去转换成游戏引擎的格式(json)
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			GuGu_LOGD("Error Assimp: %s\n", import.GetErrorString());
			return "";
		}

		m_currentLoadStaticMesh = std::make_shared<GStaticMesh>();
		m_currentStaticMeshIndicesNumber = 0;
		m_currentStaticMeshVerticesNumber = 0;
		processNode(scene->mRootNode, scene);
		
		uint64_t bufferSize = 0;
		if (!m_currentLoadStaticMesh->m_positionData.empty())
		{
			AppendBufferRange(m_currentLoadStaticMesh->getVertexBufferRange(GVertexAttribute::Position),
				m_currentLoadStaticMesh->m_positionData.size() *
				sizeof(m_currentLoadStaticMesh->m_positionData[0]),
				/*out*/bufferSize);
		}
		if (!m_currentLoadStaticMesh->m_texCoord1Data.empty())
		{
			AppendBufferRange(m_currentLoadStaticMesh->getVertexBufferRange(GVertexAttribute::TexCoord1),
				m_currentLoadStaticMesh->m_texCoord1Data.size() *
				sizeof(m_currentLoadStaticMesh->m_texCoord1Data[0]),
				/*out*/bufferSize);
		}
		if (!m_currentLoadStaticMesh->m_normalData.empty())
		{
			AppendBufferRange(m_currentLoadStaticMesh->getVertexBufferRange(GVertexAttribute::Normal),
				m_currentLoadStaticMesh->m_normalData.size() *
				sizeof(m_currentLoadStaticMesh->m_normalData[0]),
				/*out*/bufferSize);
		}

		//meta::Variant object = ObjectVariant(m_currentLoadStaticMesh.get());

		//序列化掉这个模型为json文件
		//nlohmann::json staticMeshJson = AssetManager::getAssetManager().serializeJson(object.GetType(), object);
		nlohmann::json staticMeshJson = AssetManager::getAssetManager().serializeJson(m_currentLoadStaticMesh);
		return staticMeshJson;
	}
	void ModelImporter::processNode(aiNode* node, const aiScene* scene)
	{
		//处理节点上所有网格
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}
		//对子节点重复这一过程
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			processNode(node->mChildren[i], scene);
		}
	}
	void ModelImporter::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		if (m_currentLoadStaticMesh)
		{
			std::shared_ptr<GMeshGeometry> gMeshGeometry = std::make_shared<GMeshGeometry>();//单个子网格
			gMeshGeometry->m_numVertices = mesh->mNumVertices;
			
			for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
			{
				math::float3 position;
				position.x = mesh->mVertices[i].x;
				position.y = mesh->mVertices[i].y;
				position.z = mesh->mVertices[i].z;
				m_currentLoadStaticMesh->m_positionData.push_back(position);

				if (mesh->mTextureCoords[0])
				{
					math::float2 texcoord1;
					texcoord1.x = mesh->mTextureCoords[0][i].x;
					texcoord1.y = mesh->mTextureCoords[0][i].y;
					m_currentLoadStaticMesh->m_texCoord1Data.push_back(texcoord1);
				}
				
				if (mesh->HasNormals())
				{
					math::float3 normal;
					normal.x = mesh->mNormals[i].x;
					normal.y = mesh->mNormals[i].y;
					normal.z = mesh->mNormals[i].z;
					m_currentLoadStaticMesh->m_normalData.push_back(normal);
				}

				//todo:加载更多数据
				if (m_isSkeleton)
				{
					m_currentLoadStaticMesh->m_weightData.push_back(math::float4().zero());
					m_currentLoadStaticMesh->m_jointData.push_back(math::vector<int16_t, 4>(-1, -1, -1, -1));
				}		
			}

			//是骨架网格体
			if (m_isSkeleton)
			{
				for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
				{
					int32_t boneId = -1;
					GuGuUtf8Str boneName = mesh->mBones[boneIndex]->mName.C_Str();
					if (m_currentLoadStaticMesh->m_boneInfos.find(boneName) == m_currentLoadStaticMesh->m_boneInfos.end())
					{
						BoneInfo boneInfo;
						boneInfo.m_boneId = ++m_boneStartIndex;
						boneInfo.m_offsetMatrix = ConvertMatrixToMyMathMatrix(mesh->mBones[boneIndex]->mOffsetMatrix);
						m_currentLoadStaticMesh->m_boneInfos[boneName] = boneInfo;
						boneId = m_boneStartIndex;
					}
					else
					{
						boneId = m_currentLoadStaticMesh->m_boneInfos[boneName].m_boneId;
					}

					//获取这跟骨骼对所有顶点权重的影响
					auto widgets = mesh->mBones[boneIndex]->mWeights;
					int32_t numWeights = mesh->mBones[boneIndex]->mNumWeights;

					for (uint32_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
					{
						math::float4& weight = m_currentLoadStaticMesh->m_weightData[widgets[weightIndex].mVertexId];
						math::vector<int16_t, 4>& joint = m_currentLoadStaticMesh->m_jointData[widgets[weightIndex].mVertexId];//指向骨骼矩阵的索引
						//max bone index is 4
						for (int32_t index = 0; index < 4; ++index)
						{
							if (joint[index] < 0)
							{
								joint[index] = boneId;
								weight[index] = widgets[weightIndex].mWeight;
							}
						}
					}
				}
			}

			uint32_t totalIndicesNumber = 0;
			for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];
				for (uint32_t j = 0; j < face.mNumIndices; ++j)
				{
					m_currentLoadStaticMesh->m_indexData.push_back(face.mIndices[j]);
				}
				totalIndicesNumber += face.mNumIndices;
			}
			gMeshGeometry->m_numIndices = totalIndicesNumber;
			gMeshGeometry->m_indexOffsetInMesh = m_currentStaticMeshIndicesNumber;
			gMeshGeometry->m_vertexOffsetInMesh = m_currentStaticMeshVerticesNumber;
			m_currentStaticMeshIndicesNumber += gMeshGeometry->m_numIndices;
			m_currentStaticMeshVerticesNumber += gMeshGeometry->m_numVertices;
		}
	}
}