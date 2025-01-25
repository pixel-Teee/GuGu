#include <pch.h>

#include "StaticMesh.h"

namespace GuGu {
	GStaticMesh::GStaticMesh()
	{
	}
	GStaticMesh::~GStaticMesh()
	{
	}
	void GStaticMesh::Update(float fElapsedTimeSeconds)
	{
		
	}

	meta::Type GStaticMesh::GetType() const
	{
		return typeof(GStaticMesh);
	}

	meta::Object* GStaticMesh::Clone(void) const
	{
		GStaticMesh* staticMesh = new GStaticMesh();
		staticMesh->m_indexData = m_indexData;
		staticMesh->m_positionData = m_positionData;
		staticMesh->m_texCoord1Data = m_texCoord1Data;
		staticMesh->m_texCoord2Data = m_texCoord2Data;
		staticMesh->m_normalData = m_normalData;
		staticMesh->m_tangentData = m_tangentData;
		staticMesh->m_jointData = m_jointData;//指向骨骼矩阵的索引
		staticMesh->m_weightData = m_weightData;
		staticMesh->m_vertexBufferRanges = m_vertexBufferRanges;
		staticMesh->m_geometries = m_geometries;
		staticMesh->m_totalIndices = m_totalIndices;
		staticMesh->m_totalVertices = m_totalVertices;
		staticMesh->m_objectSpaceBounds = m_objectSpaceBounds;
		return staticMesh;
	}

	void GStaticMesh::OnSerialize(nlohmann::json& output) const
	{
		output["type"] = typeidof(GStaticMesh);
		output["indexData"] = nlohmann::json::array();
		for (size_t i = 0; i < m_indexData.size(); ++i)
		{
			output["indexData"].emplace_back(m_indexData[i]);
		}
		output["positionData"] = nlohmann::json::array();
		for (size_t i = 0; i < m_positionData.size(); ++i)
		{
			output["positionData"].emplace_back(m_positionData[i].x);
			output["positionData"].emplace_back(m_positionData[i].y);
			output["positionData"].emplace_back(m_positionData[i].z);
		}
		output["texCoord1"] = nlohmann::json::array();
		for (size_t i = 0; i < m_texCoord1Data.size(); ++i)
		{
			output["texCoord1"].emplace_back(m_texCoord1Data[i].x);
			output["texCoord1"].emplace_back(m_texCoord1Data[i].y);
		}
		output["normal"] = nlohmann::json::array();
		for (size_t i = 0; i < m_normalData.size(); ++i)
		{
			output["normal"].emplace_back(m_normalData[i].x);
			output["normal"].emplace_back(m_normalData[i].y);
			output["normal"].emplace_back(m_normalData[i].z);
		}
	}

	void GStaticMesh::OnDeserialize(const nlohmann::json& input)
	{
		nlohmann::json indexData = input["indexData"];
		m_indexData.resize(indexData.size());
		for (int32_t i = 0; i < indexData.size(); ++i)
		{
			m_indexData[i] = indexData[i].get<uint32_t>();
		}
		m_totalIndices = indexData.size();
		nlohmann::json positionData = input["positionData"];
		int32_t currentIndex = 0;
		m_positionData.resize(positionData.size() / 3);
		for (int32_t i = 0; i < positionData.size(); i = i + 3)
		{
			math::float3 position = math::float3(positionData[i].get<float>(), positionData[i + 1].get<float>(), positionData[i + 2].get<float>());
			m_positionData[currentIndex] = position;
			++currentIndex;
		}
		nlohmann::json texCoord1 = input["texCoord1"];
		currentIndex = 0;
		m_texCoord1Data.resize(texCoord1.size());
		for (int32_t i = 0; i < texCoord1.size(); i = i + 2)
		{
			math::float2 texcoord = math::float2(texCoord1[i].get<float>(), texCoord1[i + 1].get<float>());
			m_texCoord1Data[currentIndex] = texcoord;
			++currentIndex;
		}
		nlohmann::json normal = input["normal"];
		currentIndex = 0;
		m_normalData.resize(normal.size());
		for (int32_t i = 0; i < normal.size(); i = i + 3)
		{
			math::float3 normaldata = math::float3(normal[i].get<float>(), normal[i + 1].get<float>(), normal[i + 2].get<float>());
			m_normalData[currentIndex] = normaldata;
			++currentIndex;
		}
		m_totalVertices = positionData.size() / 3;

		m_objectSpaceBounds = dm::box3(m_positionData.size(), m_positionData.data());
	}

	const dm::box3& GStaticMesh::getObjectSpaceBounds() const
	{
		return m_objectSpaceBounds;
	}

}