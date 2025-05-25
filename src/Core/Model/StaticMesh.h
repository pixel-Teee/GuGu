#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>

#include <Renderer/nvrhi.h>

#include "ModelBasic.h"

namespace GuGu{

	//static mesh
	class GStaticMesh : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GStaticMesh();

		virtual ~GStaticMesh();

		bool hasAttribute(GVertexAttribute attr) { return m_vertexBufferRanges[int32_t(attr)].byteSize != 0; }
		nvrhi::BufferRange& getVertexBufferRange(GVertexAttribute attr) { return m_vertexBufferRanges[int32_t(attr)]; }
		const nvrhi::BufferRange& getVertexBufferRange(GVertexAttribute attr) const { return m_vertexBufferRanges[int32_t(attr)]; }

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		const dm::box3& getObjectSpaceBounds() const;
	
		//组成一个模型的所有子网格的数据(包括内存数据和显存数据)
		nvrhi::BufferHandle m_indexBuffer;
		nvrhi::BufferHandle m_vertexBuffer;

		//内存数据是要序列化的
		Array<uint32_t> m_indexData;
		Array<math::float3> m_positionData;
		Array<math::float2> m_texCoord1Data;
		Array<math::float2> m_texCoord2Data;
		Array<math::float3> m_normalData;
		Array<math::float3> m_tangentData;
		Array<math::vector<uint16_t, 4>> m_jointData;//指向骨骼矩阵的索引
		Array<math::float4> m_weightData;
		std::array<nvrhi::BufferRange, size_t(GVertexAttribute::Count)> m_vertexBufferRanges;

		std::vector<std::shared_ptr<GMeshGeometry>> m_geometries;//描述整个模型网格中的子网格
		dm::box3 m_objectSpaceBounds;
		uint32_t m_totalIndices = 0;
		uint32_t m_totalVertices = 0;
	};
}