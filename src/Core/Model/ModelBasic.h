#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	enum class GVertexAttribute //顶点属性
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

	struct GMeshGeometry
	{
		uint32_t m_indexOffsetInMesh = 0;
		uint32_t m_vertexOffsetInMesh = 0;
		dm::box3 m_objectSpaceBounds;
		uint32_t m_numIndices = 0;
		uint32_t m_numVertices = 0;
	};
}