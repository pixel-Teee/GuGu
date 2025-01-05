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
		throw std::logic_error("The method or operation is not implemented.");
	}

	void GStaticMesh::OnSerialize(nlohmann::json& output) const
	{
		nlohmann::json object{};
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
		output["GStaticMesh"] = object;
	}

	void GStaticMesh::OnDeserialize(const nlohmann::json& input)
	{
		nlohmann::json indexData = input["indexData"];
		for (int32_t i = 0; i < indexData.size(); ++i)
		{
			m_indexData[i] = indexData[i].get<uint32_t>();
		}
		nlohmann::json positionData = input["indexData"];
		int32_t currentIndex = 0;
		for (int32_t i = 0; i < positionData.size(); i = i + 3)
		{
			m_positionData[currentIndex] = positionData[i].get<float>();
			m_positionData[currentIndex + 1] = positionData[i + 1].get<float>();
			m_positionData[currentIndex + 2] = positionData[i + 2].get<float>();
			currentIndex = currentIndex + 3;
		}
		nlohmann::json texCoord1 = input["texCoord1"];
		currentIndex = 0;
		for (int32_t i = 0; i < texCoord1.size(); i = i + 3)
		{
			m_texCoord1Data[currentIndex] = texCoord1[i].get<float>();
			m_texCoord1Data[currentIndex + 1] = texCoord1[i + 1].get<float>();
			m_texCoord1Data[currentIndex + 2] = texCoord1[i + 2].get<float>();
			currentIndex = currentIndex + 3;
		}
		nlohmann::json normal = input["normal"];
		currentIndex = 0;
		for (int32_t i = 0; i < normal.size(); i = i + 3)
		{
			m_normalData[currentIndex] = normal[i].get<float>();
			m_normalData[currentIndex + 1] = normal[i + 1].get<float>();
			m_normalData[currentIndex + 2] = normal[i + 2].get<float>();
			currentIndex = currentIndex + 3;
		}
	}

}