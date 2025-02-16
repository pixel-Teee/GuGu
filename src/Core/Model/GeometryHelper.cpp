#include <pch.h>

#include "GeometryHelper.h"
#include <Core/Math/MyMath.h>

namespace GuGu {
	GStaticMesh GeometryHelper::createCylinder(float bottomRaidus, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, math::float4x4 transform)
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

				math::float3 position = math::float4(r * c, y, r * s, 1.0f) * transform;
				math::float2 texcoord = math::float2(j / sliceCount, i / stackCount);
				math::float3 tangent = math::float3(-s, 0.0f, c) * math::float3x3(transform);

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
		createCylinderTopCap(staticMesh, bottomRaidus, topRadius, height, sliceCount, stackCount, transform);
		createCylinderBottomCap(staticMesh, bottomRaidus, topRadius, height, sliceCount, stackCount, transform);
		staticMesh.m_totalIndices = staticMesh.m_indexData.size();
		staticMesh.m_objectSpaceBounds = dm::box3(staticMesh.m_positionData.size(), staticMesh.m_positionData.data());
		return staticMesh;
	}
	void GeometryHelper::createCylinderTopCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, math::float4x4 transform)
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

			staticMesh.m_positionData.push_back(math::float4(x, y, z, 1.0f) * transform);
			staticMesh.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f) * math::float3x3(transform));
			staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f) * math::float3x3(transform));
			staticMesh.m_texCoord1Data.push_back(math::float2(u, v));
		}

		//cap 的中心顶点
		staticMesh.m_positionData.push_back(math::float4(0.0, y, 0.0, 1.0f) * transform);
		staticMesh.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f) * math::float3x3(transform));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f) * math::float3x3(transform));
		staticMesh.m_texCoord1Data.push_back(math::float2(0.5f, 0.5f));

		uint32_t centerIndex = (uint32_t)staticMesh.m_positionData.size() - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			staticMesh.m_indexData.push_back(centerIndex);
			staticMesh.m_indexData.push_back(baseIndex + i + 1);
			staticMesh.m_indexData.push_back(baseIndex + i);
		}
	}
	void GeometryHelper::createCylinderBottomCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount, math::float4x4 transform)
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

			staticMesh.m_positionData.push_back(math::float4(x, y, z, 1.0f) * transform);
			staticMesh.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f) * math::float3x3(transform));
			staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f) * math::float3x3(transform));
			staticMesh.m_texCoord1Data.push_back(math::float2(u, v));
		}

		//cap 的中心顶点
		staticMesh.m_positionData.push_back(math::float4(0.0, y, 0.0, 1.0f) * transform);
		staticMesh.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f) * math::float3x3(transform));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f) * math::float3x3(transform));
		staticMesh.m_texCoord1Data.push_back(math::float2(0.5f, 0.5f));

		uint32_t centerIndex = (uint32_t)staticMesh.m_positionData.size() - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			staticMesh.m_indexData.push_back(centerIndex);
			staticMesh.m_indexData.push_back(baseIndex + i);
			staticMesh.m_indexData.push_back(baseIndex + i + 1);
		}
	}
	GStaticMesh GeometryHelper::createQuat(float x, float y, float w, float h, float depth)
	{
		GStaticMesh staticMesh;
		staticMesh.m_positionData.resize(4);
		staticMesh.m_indexData.resize(6);

		//位置坐标描述在NDC空间中
		staticMesh.m_positionData.push_back(math::float3(x, y, depth));
		staticMesh.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		staticMesh.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		staticMesh.m_positionData.push_back(math::float3(x + w, y, depth));
		staticMesh.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		staticMesh.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));

		staticMesh.m_positionData.push_back(math::float3(x + w, y - h, depth));
		staticMesh.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		staticMesh.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));

		staticMesh.m_indexData.push_back(0);
		staticMesh.m_indexData.push_back(1);
		staticMesh.m_indexData.push_back(2);

		staticMesh.m_indexData.push_back(0);
		staticMesh.m_indexData.push_back(2);
		staticMesh.m_indexData.push_back(3);

		return staticMesh;
	}
	GStaticMesh GeometryHelper::createGrid(float width, float depth, uint32_t m, uint32_t n)
	{
		GStaticMesh staticMesh;
		uint32_t vertexCount = m * n;
		uint32_t faceCount = (m - 1) * (n - 1) * 2;

		float halfWidth = 0.5f * width;
		float halfDepth = 0.5f * depth;

		float dx = width / (n - 1);
		float dz = depth / (n - 1);

		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		staticMesh.m_positionData.resize(vertexCount);

		for (uint32_t i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dz;
			for (uint32_t j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;

				staticMesh.m_positionData.push_back(math::float3(x, 0.0f, z));
				staticMesh.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
				staticMesh.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
				staticMesh.m_texCoord1Data.push_back(math::float2(j * du, i * dv));
			}
		}

		staticMesh.m_indexData.push_back(faceCount * 3);

		uint32_t k = 0;
		for (uint32_t i = 0; i < m - 1; ++i)
		{
			for (uint32_t j = 0; j < n - 1; ++j)
			{
				staticMesh.m_indexData.push_back(i * n + j);
				staticMesh.m_indexData.push_back(i * n + j + 1);
				staticMesh.m_indexData.push_back((i + 1) * n + j);

				staticMesh.m_indexData.push_back((i + 1) * n + j);
				staticMesh.m_indexData.push_back(i * n + j + 1);
				staticMesh.m_indexData.push_back((i + 1) * n + j + 1);
			}
		}

		return staticMesh;
	}

	void GeometryHelper::subdivide(GStaticMesh& staticMesh, uint32_t numSubdivisions)
	{
		Array<math::float3> positionData = staticMesh.m_positionData;
		Array<math::float3> normalData = staticMesh.m_normalData;
		Array<math::float3> tangentData = staticMesh.m_tangentData;
		Array<math::float2> texcoordData = staticMesh.m_texCoord1Data;
		Array<uint32_t> indexData = staticMesh.m_indexData;

		//copy 
		staticMesh.m_positionData.resize(0);
		staticMesh.m_normalData.resize(0);
		staticMesh.m_tangentData.resize(0);
		staticMesh.m_texCoord1Data.resize(0);
		staticMesh.m_indexData.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2
		uint32_t numTris = (uint32_t)indexData.size() / 3;
		for (uint32_t i = 0; i < numTris; ++i)
		{
			//vertex 0
			math::float3 positionData1 = positionData[indexData[i * 3 + 0]];
			math::float3 normalData1 = normalData[indexData[i * 3 + 0]];
			math::float3 tangentData1 = tangentData[indexData[i * 3 + 0]];
			math::float2 texCoordData1 = texcoordData[indexData[i * 3 + 0]];

			//vertex 1
			math::float3 positionData2 = positionData[indexData[i * 3 + 1]];
			math::float3 normalData2 = normalData[indexData[i * 3 + 1]];
			math::float3 tangentData2 = tangentData[indexData[i * 3 + 1]];
			math::float2 texCoordData2 = texcoordData[indexData[i * 3 + 1]];

			//vertex 2
			math::float3 positionData3 = positionData[indexData[i * 3 + 2]];
			math::float3 normalData3 = normalData[indexData[i * 3 + 2]];
			math::float3 tangentData3 = tangentData[indexData[i * 3 + 2]];
			math::float2 texCoordData3 = texcoordData[indexData[i * 3 + 2]];

			math::float3 midPositionData1;
			math::float3 midNormalData1;
			math::float3 midtangentData1;
			math::float2 midtexcoordData1;
			midPoint(positionData1, positionData2,
				tangentData1, tangentData2,
				normalData1, normalData2,
				texCoordData1, texCoordData2,
				midPositionData1,
				midtangentData1,
				midNormalData1,
				midtexcoordData1);

			math::float3 midPositionData2;
			math::float3 midNormalData2;
			math::float3 midtangentData2;
			math::float2 midtexcoordData2;
			midPoint(positionData2, positionData3,
				tangentData2, tangentData3,
				normalData2, normalData3,
				texCoordData2, texCoordData3,
				midPositionData2,
				midtangentData2,
				midNormalData2,
				midtexcoordData2);

			math::float3 midPositionData3;
			math::float3 midNormalData3;
			math::float3 midtangentData3;
			math::float2 midtexcoordData3;
			midPoint(positionData1, positionData3,
				tangentData1, tangentData3,
				normalData1, normalData3,
				texCoordData1, texCoordData3,
				midPositionData3,
				midtangentData3,
				midNormalData3,
				midtexcoordData3);

			staticMesh.m_positionData.push_back(positionData1);
			staticMesh.m_positionData.push_back(positionData2);
			staticMesh.m_positionData.push_back(positionData3);
			staticMesh.m_positionData.push_back(midPositionData1);
			staticMesh.m_positionData.push_back(midPositionData2);
			staticMesh.m_positionData.push_back(midPositionData3);
			staticMesh.m_normalData.push_back(normalData1);
			staticMesh.m_normalData.push_back(normalData2);
			staticMesh.m_normalData.push_back(normalData3);
			staticMesh.m_normalData.push_back(midNormalData1);
			staticMesh.m_normalData.push_back(midNormalData2);
			staticMesh.m_normalData.push_back(midNormalData3);
			staticMesh.m_tangentData.push_back(tangentData1);
			staticMesh.m_tangentData.push_back(tangentData2);
			staticMesh.m_tangentData.push_back(tangentData3);
			staticMesh.m_tangentData.push_back(midtangentData1);
			staticMesh.m_tangentData.push_back(midtangentData2);
			staticMesh.m_tangentData.push_back(midtangentData3);
			staticMesh.m_texCoord1Data.push_back(texCoordData1);
			staticMesh.m_texCoord1Data.push_back(texCoordData2);
			staticMesh.m_texCoord1Data.push_back(texCoordData3);
			staticMesh.m_texCoord1Data.push_back(midtexcoordData1);
			staticMesh.m_texCoord1Data.push_back(midtexcoordData2);
			staticMesh.m_texCoord1Data.push_back(midtexcoordData3);

			staticMesh.m_indexData.push_back(i * 6 + 0);
			staticMesh.m_indexData.push_back(i * 6 + 3);
			staticMesh.m_indexData.push_back(i * 6 + 5);

			staticMesh.m_indexData.push_back(i * 6 + 3);
			staticMesh.m_indexData.push_back(i * 6 + 4);
			staticMesh.m_indexData.push_back(i * 6 + 5);

			staticMesh.m_indexData.push_back(i * 6 + 5);
			staticMesh.m_indexData.push_back(i * 6 + 4);
			staticMesh.m_indexData.push_back(i * 6 + 2);

			staticMesh.m_indexData.push_back(i * 6 + 3);
			staticMesh.m_indexData.push_back(i * 6 + 1);
			staticMesh.m_indexData.push_back(i * 6 + 4);
		}
	}

	GStaticMesh GeometryHelper::createBox(float width, float height, float depth, uint32_t numSubdivisions, math::float4x4 transform)
	{
		GStaticMesh box;

		float w2 = 0.5f * width;//half width
		float h2 = 0.5f * height;//half height
		float d2 = 0.5f * depth;//half depth

		//front face
		box.m_positionData.push_back(math::float4(-w2, -h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 1.0f));

		box.m_positionData.push_back(math::float4(-w2, +h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		box.m_positionData.push_back(math::float4(+w2, +h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));

		box.m_positionData.push_back(math::float4(+w2, -h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));
		//back face
		box.m_positionData.push_back(math::float4(-w2, -h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));

		box.m_positionData.push_back(math::float4(+w2, -h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 1.0f));

		box.m_positionData.push_back(math::float4(+w2, +h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		box.m_positionData.push_back(math::float4(-w2, +h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));
		//top face
		box.m_positionData.push_back(math::float4(-w2, +h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 1.0f));

		box.m_positionData.push_back(math::float4(-w2, +h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		box.m_positionData.push_back(math::float4(+w2, +h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));

		box.m_positionData.push_back(math::float4(+w2, +h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, 1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));

		//bottom face
		box.m_positionData.push_back(math::float4(-w2, -h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));

		box.m_positionData.push_back(math::float4(+w2, -h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 1.0f));

		box.m_positionData.push_back(math::float4(+w2, -h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		box.m_positionData.push_back(math::float4(-w2, -h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(0.0f, -1.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));
		//left face
		box.m_positionData.push_back(math::float4(-w2, -h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 1.0f));

		box.m_positionData.push_back(math::float4(-w2, +h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		box.m_positionData.push_back(math::float4(-w2, +h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));

		box.m_positionData.push_back(math::float4(-w2, -h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(-1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, -1.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));
		//right face
		box.m_positionData.push_back(math::float4(+w2, -h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 1.0f));

		box.m_positionData.push_back(math::float4(+w2, +h2, -d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_texCoord1Data.push_back(math::float2(0.0f, 0.0f));

		box.m_positionData.push_back(math::float4(+w2, +h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 0.0f));

		box.m_positionData.push_back(math::float4(+w2, -h2, +d2, 1.0f) * transform);
		box.m_normalData.push_back(math::float3(1.0f, 0.0f, 0.0f));
		box.m_tangentData.push_back(math::float3(0.0f, 0.0f, 1.0f));
		box.m_texCoord1Data.push_back(math::float2(1.0f, 1.0f));

		//indices
		box.m_indexData.resize(36);
		//front
		box.m_indexData[0] = 0;
		box.m_indexData[1] = 1;
		box.m_indexData[2] = 2;
		box.m_indexData[3] = 0;
		box.m_indexData[4] = 2;
		box.m_indexData[5] = 3;
		//back
		box.m_indexData[6] = 4;
		box.m_indexData[7] = 5;
		box.m_indexData[8] = 6;
		box.m_indexData[9] = 4;
		box.m_indexData[10] = 6;
		box.m_indexData[11] = 7;
		//top
		box.m_indexData[12] = 8;
		box.m_indexData[13] = 9;
		box.m_indexData[14] = 10;
		box.m_indexData[15] = 8;
		box.m_indexData[16] = 10;
		box.m_indexData[17] = 11;
		//bottom
		box.m_indexData[18] = 12;
		box.m_indexData[19] = 13;
		box.m_indexData[20] = 14;
		box.m_indexData[21] = 12;
		box.m_indexData[22] = 14;
		box.m_indexData[23] = 15;
		//left
		box.m_indexData[24] = 16;
		box.m_indexData[25] = 17;
		box.m_indexData[26] = 18;
		box.m_indexData[27] = 16;
		box.m_indexData[28] = 18;
		box.m_indexData[29] = 19;
		//right
		box.m_indexData[30] = 20;
		box.m_indexData[31] = 21;
		box.m_indexData[32] = 22;
		box.m_indexData[33] = 20;
		box.m_indexData[34] = 22;
		box.m_indexData[35] = 23;

		//细分次数
		numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);

		for (uint32_t i = 0; i < numSubdivisions; ++i)
			subdivide(box, numSubdivisions);

		box.m_totalIndices = box.m_indexData.size();
		box.m_totalVertices = box.m_positionData.size();
		box.m_objectSpaceBounds = dm::box3(box.m_positionData.size(), box.m_positionData.data());

		return box;
	}

	void GeometryHelper::midPoint(math::float3 position1, math::float3 position2, math::float3 tangent1, math::float3 tangent2, math::float3 normal1, math::float3 normal2, math::float2 texcoord1, math::float2 texcoord2, math::float3& midPosition, math::float3& midTangent, math::float3& midNormal, math::float2& midTexcoord)
	{
		midPosition = 0.5f * (position1 + position2);
		midNormal = math::normalize(0.5f * (normal1 + normal2));
		midTangent = math::normalize(0.5f * (tangent1 + tangent2));
		midTexcoord = 0.5f * (tangent1 + tangent2);
	}

	GStaticMesh GeometryHelper::createToru(float R, float r, int32_t majorSegments, int32_t minorSegments, math::float4x4 transform)
	{
		GStaticMesh toru;

		for (int i = 0; i <= majorSegments; ++i) {
			float theta = (float)i / majorSegments * 2 * math::PI_f;
			float cosTheta = std::cos(theta);
			float sinTheta = std::sin(theta);

			for (int j = 0; j <= minorSegments; ++j) {
				float phi = (float)j / minorSegments * 2 * math::PI_f;
				float cosPhi = std::cos(phi);
				float sinPhi = std::sin(phi);

				// 计算顶点位置
				float x = (R + r * cosPhi) * cosTheta;
				float y = r * sinPhi;
				float z = (R + r * cosPhi) * sinTheta;

				// 计算法线（单位向量）
				float nx = cosTheta * cosPhi;
				float ny = sinPhi;
				float nz = sinTheta * cosPhi;

				// 纹理坐标
				float u = (float)i / majorSegments; // 沿主圆方向
				float v = (float)j / minorSegments; // 沿管圆方向

				toru.m_positionData.push_back(math::float4(x, y, z, 1.0f) * transform);
				toru.m_normalData.push_back(math::float3(nx, ny, nz ) * math::float3x3(transform));
				toru.m_texCoord1Data.push_back({ u, v });
				toru.m_tangentData.push_back(math::normalize(math::cross(toru.m_positionData.back(), toru.m_normalData.back())) * math::float3x3(transform));
			}
		}

		// 生成索引数据
		for (int i = 0; i < majorSegments; ++i) {
			for (int j = 0; j < minorSegments; ++j) {
				// 当前四边形的四个顶点索引
				int i0 = i * (minorSegments + 1) + j;
				int i1 = (i + 1) * (minorSegments + 1) + j;
				int i2 = (i + 1) * (minorSegments + 1) + (j + 1);
				int i3 = i * (minorSegments + 1) + (j + 1);

				// 划分为两个三角形
				
				toru.m_indexData.push_back(i2);
				toru.m_indexData.push_back(i1);
				toru.m_indexData.push_back(i0);

				
				toru.m_indexData.push_back(i3);
				toru.m_indexData.push_back(i2);
				toru.m_indexData.push_back(i0);
			}
		}

		toru.m_totalIndices = toru.m_indexData.size();
		toru.m_totalVertices = toru.m_positionData.size();
		toru.m_objectSpaceBounds = dm::box3(toru.m_positionData.size(), toru.m_positionData.data());

		return toru;
	}
}