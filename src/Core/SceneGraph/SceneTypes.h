#pragma once

#include <Core/GuGuUtf8Str.h>

#include <Renderer/nvrhi.h>

#include <Core/Math/MyMath.h>

namespace GuGu {
	enum class VertexAttribute
	{
		Position,
		TexCoord1,
		TexCoord2,
		Normal,
		Tangent,
		JointIndices,
		JointWeights,
		Count
	};

	struct BufferGroup
	{
		nvrhi::BufferHandle indexBuffer;
		nvrhi::BufferHandle vertexBuffer;
		nvrhi::BufferHandle instanceBuffer;
		std::vector<uint32_t> indexData;
		std::vector<math::float3> positionData;
		std::vector<math::float2> texCoord1Data;
		std::vector<math::vector<uint16_t, 4>> jointData;//bone matrix index
		std::vector<math::float4> weightData;
		std::vector<uint32_t> normalData;
		std::vector<uint32_t> tangentData;
		std::array<nvrhi::BufferRange, size_t(VertexAttribute::Count)> vertexBufferRanges;
		bool hasAttribute(VertexAttribute attr) const { return vertexBufferRanges[int32_t(attr)].byteSize != 0; }
		nvrhi::BufferRange& getVertexBufferRange(VertexAttribute attr) { return vertexBufferRanges[int32_t(attr)]; }
		const nvrhi::BufferRange& getVertexBufferRange(VertexAttribute attr) const { return vertexBufferRanges[int32_t(attr)]; }
	};
	struct MeshGeometry
	{
		uint32_t indexOffsetInMesh = 0;
		uint32_t vertexOffsetInMesh = 0;
		dm::box3 objectSpaceBounds;
		uint32_t numIndices = 0;
		uint32_t numVertices = 0;
		
		virtual ~MeshGeometry() = default;
	};
	struct MeshInfo
	{
		GuGuUtf8Str name;
		std::shared_ptr<BufferGroup> buffers;
		std::shared_ptr<MeshInfo> skinPrototype;
		std::vector<std::shared_ptr<MeshGeometry>> geometries;
		dm::box3 objectSpaceBounds;
		uint32_t indexOffset = 0;
		uint32_t vertexOffset = 0;
		uint32_t totalIndices = 0;
		uint32_t totalVertices = 0;

		virtual ~MeshInfo() = default;
	};
}