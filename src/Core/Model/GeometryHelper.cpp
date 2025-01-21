#include <pch.h>

#include "GeometryHelper.h"
#include <Core/Math/MyMath.h>

namespace GuGu {
	GStaticMesh GeometryHelper::createCylinder(float bottomRaidus, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		//构建堆叠层
		float stackHeight = height / stackCount;
		float radiusStep = (topRadius - bottomRaidus) / stackCount;//每一层，半径的变化

		uint32_t ringCount = stackCount + 1;

		GStaticMesh staticMesh;
		//从底面开始，由下至上计算每个堆叠层环上的顶点坐标
		for (uint32_t i = 0; i < ringCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight;
			float r = bottomRaidus + i * radiusStep;

			//环上的各个顶点，每个片的增量
			float dTheta = 2.0f * math::PI_d / sliceCount;
			for (uint32_t j = 0; j <= sliceCount; ++j)
			{
				float c = std::cosf(j * dTheta);
				float s = std::sinf(j * dTheta);

				math::float3 position = math::float3(r * c, y, r * s);
				math::float2 texcoord = math::float2(j / sliceCount, i / stackCount);
				math::float3 tangent = math::float3(-s, 0.0f, c);

				float dr = bottomRaidus - topRadius;
				math::float3 bitangent(dr * c, -height, dr * s);

				math::float3 normal = math::normalize(math::cross(tangent, bitangent));

				staticMesh.m_positionData.push_back(position);
				staticMesh.m_texCoord1Data.push_back(texcoord);
				staticMesh.m_tangentData.push_back(tangent);
				staticMesh.m_normalData.push_back(normal);
			}
		}

		uint32_t ringVertexCount = sliceCount + 1;
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			for (uint32_t j = 0; j < sliceCount; ++j)
			{
				staticMesh.m_indexData.push_back(i * ringVertexCount + j);
				staticMesh.m_indexData.push_back((i + 1) * ringVertexCount + j);
				staticMesh.m_indexData.push_back((i + 1) * ringVertexCount + j + 1);
	
				staticMesh.m_indexData.push_back(i * ringVertexCount + j);
				staticMesh.m_indexData.push_back((i + 1) * ringVertexCount + j + 1);
				staticMesh.m_indexData.push_back(i * ringVertexCount + j + 1);
			}
		}
		createCylinderTopCap(staticMesh, bottomRaidus, topRadius, height, sliceCount, stackCount);
		createCylinderBottomCap(staticMesh, bottomRaidus, topRadius, height, sliceCount, stackCount);
		staticMesh.m_totalIndices = staticMesh.m_indexData.size();
		return staticMesh;
	}
	void GeometryHelper::createCylinderTopCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		uint32_t baseIndex = (uint32_t)staticMesh.m_positionData.size();

		float y = 0.5f * height;
		float dTheta = 2.0f * math::PI_d / sliceCount;

		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			staticMesh.m_positionData.push_back(math::float3(x, y, z));
			staticMesh.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
			staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
			staticMesh.m_texCoord1Data.push_back(math::float2(u, v));
		}

		//cap 的中心顶点
		staticMesh.m_positionData.push_back(math::float3(0.0, y, 0.0));
		staticMesh.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		staticMesh.m_texCoord1Data.push_back(math::float2(0.5f, 0.5f));

		uint32_t centerIndex = (uint32_t)staticMesh.m_positionData.size() - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			staticMesh.m_indexData.push_back(centerIndex);
			staticMesh.m_indexData.push_back(baseIndex + i + 1);
			staticMesh.m_indexData.push_back(baseIndex + i);
		}
	}
	void GeometryHelper::createCylinderBottomCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		uint32_t baseIndex = (uint32_t)staticMesh.m_positionData.size();

		float y = -0.5f * height;
		float dTheta = 2.0f * math::PI_d / sliceCount;

		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRaidus * cosf(i * dTheta);
			float z = bottomRaidus * sinf(i * dTheta);

			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			staticMesh.m_positionData.push_back(math::float3(x, y, z));
			staticMesh.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f));
			staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
			staticMesh.m_texCoord1Data.push_back(math::float2(u, v));
		}

		//cap 的中心顶点
		staticMesh.m_positionData.push_back(math::float3(0.0, y, 0.0));
		staticMesh.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		staticMesh.m_texCoord1Data.push_back(math::float2(0.5f, 0.5f));

		uint32_t centerIndex = (uint32_t)staticMesh.m_positionData.size() - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			staticMesh.m_indexData.push_back(centerIndex);
			staticMesh.m_indexData.push_back(baseIndex + i);
			staticMesh.m_indexData.push_back(baseIndex + i + 1);
		}
	}
}