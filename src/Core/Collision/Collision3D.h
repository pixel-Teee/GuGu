#pragma once

#include <Core/Math/MyMath.h>
#include <vector>

namespace GuGu {
	class GameObject;
	class GStaticMesh;
	class GTexture;
	class Collision3D
	{
	public:
		Collision3D() {}

		~Collision3D() {}

		static std::shared_ptr<GameObject> pick(uint32_t x, uint32_t y,
						 uint32_t clientWidth, uint32_t clientHeight, 
						 math::float4x4 perspectiveMatrix, 
						 math::float4x4 viewMatrix,
						 const std::vector<std::shared_ptr<GameObject>>& objects, math::float4& debugDrawWorldPos);

		static std::shared_ptr<GStaticMesh> pick(uint32_t x, uint32_t y,
			uint32_t clientWidth, uint32_t clientHeight,
			math::float4x4 perspectiveMatrix,
			math::float4x4 viewMatrix,
			const std::vector<std::shared_ptr<GStaticMesh>>& objects, math::float4x4 selectedObjectTransform, math::float4& debugDrawWorldPos);

		static std::shared_ptr<GameObject> pickUIGameObject(uint32_t x, uint32_t y,
			uint32_t clientWidth, uint32_t clientHeight,
			math::float4x4 perspectiveMatrix,
			math::float4x4 viewMatrix,
			const std::vector<std::shared_ptr<GameObject>>& objects);

		static bool intersectsWithPlane(math::float3 rayOrigin, math::float3 rayDir, math::float3 planeNormal, math::float3 planePoint, math::float3& res);

		static void calculateRayOriginAndRayDir(uint32_t x, uint32_t y,
			uint32_t clientWidth, uint32_t clientHeight,
			math::float4x4 perspectiveMatrix,
			math::float4x4 viewMatrix, math::float3& outRayOrigin, math::float3& outRayDir);

		//AABB box
		static bool intersectsWithBox(math::float3 rayOrigin, math::float3 rayDir, float& dist, dm::box3 boundingBox);

		static bool intersectWithTriangle(math::float3 rayOrigin, math::float3 rayDir, math::float3 position0, math::float3 position1, math::float3 position2, float& dist);

		static void getTerrainPosition(math::float3 position0, math::float3 position1, math::float3 position2,
		math::float3& position3,
		math::float3& position4,
		math::float3& position5, math::float2 offsetXZ, math::float2 beginXZ, std::shared_ptr<GTexture> heightTexture);
	};
}