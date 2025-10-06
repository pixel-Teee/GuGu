#include <pch.h>

#include "StaticMesh.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

namespace GuGu {
	static bool registerGuGuGStaticMesh()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		{
			auto id = db.AllocateType("GuGu::GStaticMesh");
			auto& type = db.types[id];
			meta::TypeInfo<GStaticMesh>::Register(id, type, true, "C48E6CAE-2E8A-4CD9-A35D-56BF134431C7");

			auto typeID = typeidof(GStaticMesh);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GStaticMesh>::Defined)
			{
				auto& type = db.types[typeID];

				//array constructor
				type.SetArrayConstructor<GStaticMesh>();

				type.AddConstructor<GStaticMesh, false, false>({});

				type.AddConstructor<GStaticMesh, true, true>({});

				type.LoadBaseClasses(db, id, { typeof(meta::Object) });

				meta::TypeInfo<GStaticMesh>::Defined = true;
			}

			{
				auto id = db.AllocateType("std::shared_ptr<GuGu::GStaticMesh>");
				auto& type = db.types[id];
				meta::TypeInfo<std::shared_ptr<GStaticMesh>>::Register(id, type, false, "0D9E6859-B7CD-476C-AF4B-82462328432C");
			}

			{
				auto id = db.AllocateType("std::weak_ptr<GuGu::GStaticMesh>");
				auto& type = db.types[id];
				meta::TypeInfo<std::weak_ptr<GStaticMesh>>::Register(id, type, false, "37092C3B-F3B7-44FA-9508-B0FE6B6D0920");
			}
		}
		return true;
	}
	static bool registerGuGuGStaticMeshFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();

		auto& type = db.types[typeof(GStaticMesh).GetID()];	

		type.AddField<GStaticMesh, Array<uint32_t>>("m_indexData",
			(meta::FieldGetter<GStaticMesh, Array<uint32_t>, false>::Signature) & GStaticMesh::m_indexData,
			(meta::FieldSetter<GStaticMesh, Array<uint32_t>, false>::Signature) & GStaticMesh::m_indexData, {});
		type.AddField<GStaticMesh, Array<math::float3>>("m_positionData",
			(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_positionData,
			(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_positionData, {});
		type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord1Data",
			(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord1Data,
			(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord1Data, {});
		type.AddField<GStaticMesh, Array<math::float2>>("m_texCoord2Data",
			(meta::FieldGetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord2Data,
			(meta::FieldSetter<GStaticMesh, Array<math::float2>, false>::Signature) & GStaticMesh::m_texCoord2Data, {});
		type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
			(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData,
			(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData, {});
		type.AddField<GStaticMesh, Array<math::float3>>("m_tangentData",
			(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_tangentData,
			(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_tangentData, {});
		type.AddField<GStaticMesh, Array<math::float3>>("m_normalData",
			(meta::FieldGetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData,
			(meta::FieldSetter<GStaticMesh, Array<math::float3>, false>::Signature) & GStaticMesh::m_normalData, {});
		type.AddField<GStaticMesh, Array<math::vector<int16_t, 4>>>("m_jointData",
			(meta::FieldGetter<GStaticMesh, Array<math::vector<int16_t, 4>>, false>::Signature) & GStaticMesh::m_jointData,
			(meta::FieldSetter<GStaticMesh, Array<math::vector<int16_t, 4>>, false>::Signature) & GStaticMesh::m_jointData, {});
		type.AddField<GStaticMesh, Array<math::float4>>("m_weightData",
			(meta::FieldGetter<GStaticMesh, Array<math::float4>, false>::Signature) & GStaticMesh::m_weightData,
			(meta::FieldSetter<GStaticMesh, Array<math::float4>, false>::Signature) & GStaticMesh::m_weightData, {});
		type.AddField<GStaticMesh, Array<BoneInfo>>("m_boneInfoArray",
			(meta::FieldGetter<GStaticMesh, Array<BoneInfo>, false>::Signature) & GStaticMesh::m_boneInfoArray,
			(meta::FieldSetter<GStaticMesh, Array<BoneInfo>, false>::Signature) & GStaticMesh::m_boneInfoArray, {});
		type.AddField<GStaticMesh, Array<GMeshGeometry>>("m_geometries",
			(meta::FieldGetter<GStaticMesh, Array<GMeshGeometry>, false>::Signature) & GStaticMesh::m_geometries,
			(meta::FieldSetter<GStaticMesh, Array<GMeshGeometry>, false>::Signature) & GStaticMesh::m_geometries, {});

		type.AddField<GStaticMesh, bool>("m_bIsSkeletonMesh",
			(meta::FieldGetter<GStaticMesh, bool, false>::Signature) & GStaticMesh::m_bIsSkeletonMesh,
			(meta::FieldSetter<GStaticMesh, bool, false>::Signature) & GStaticMesh::m_bIsSkeletonMesh, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GStaticMesh)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGStaticMesh)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GStaticMesh)
		if (!ms_priority2.addPriorityThan(&mathfloat2Priority)) return 0;
		if (!ms_priority2.addPriorityThan(&mathfloat3Priority)) return 0;
		if (!ms_priority2.addPriorityThan(&mathfloat4Priority)) return 0;
		if (!ms_priority2.addPriorityThan(&mathuint16_4Priority)) return 0;
		if (!ms_priority2.addPriorityThan(&mathint16_4Priority)) return 0;
		if (!ms_priority2.addPriorityThan(&boneInfoPriority)) return 0;
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGStaticMeshFields)
	IMPLEMENT_INITIAL_FIELDS_END
	GStaticMesh::GStaticMesh()
	{
	}
	GStaticMesh::~GStaticMesh()
	{
	}

	void GStaticMesh::PostLoad()
	{
		m_objectSpaceBounds = dm::box3(m_positionData.size(), m_positionData.data());
		m_totalIndices = m_indexData.size();
		m_totalVertices = m_positionData.size();
		for (int32_t i = 0; i < m_boneInfoArray.size(); ++i)
		{
			m_boneInfos.insert({ m_boneInfoArray[i].m_boneName, m_boneInfoArray[i] });
		}

		//normalize
		for (int32_t i = 0; i < m_weightData.size(); ++i)
		{
			float totalWeight = m_weightData[i].x + m_weightData[i].y + m_weightData[i].z + m_weightData[i].w;
			m_weightData[i].x = m_weightData[i].x / totalWeight;
			m_weightData[i].y = m_weightData[i].y / totalWeight;
			m_weightData[i].z = m_weightData[i].z / totalWeight;
			m_weightData[i].w = m_weightData[i].w / totalWeight;
		}
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
		staticMesh->m_boneInfoArray = m_boneInfoArray;
		staticMesh->m_boneInfos = m_boneInfos;
		staticMesh->m_bIsSkeletonMesh = m_bIsSkeletonMesh;
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