#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class GameObject;
	class GStaticMesh;
	class Collision3D
	{
	public:
		Collision3D() {}

		~Collision3D() {}

		static std::shared_ptr<GameObject> pick(uint32_t x, uint32_t y,
						 uint32_t clientWidth, uint32_t clientHeight, 
						 math::float4x4 perspectiveMatrix, 
						 math::float4x4 viewMatrix,
						 const std::vector<std::shared_ptr<GameObject>>& objects, math::float4& debugDrawWorldPos = math::float4(1.0f));

		static std::shared_ptr<GStaticMesh> pick(uint32_t x, uint32_t y,
			uint32_t clientWidth, uint32_t clientHeight,
			math::float4x4 perspectiveMatrix,
			math::float4x4 viewMatrix,
			const std::vector<std::shared_ptr<GStaticMesh>>& objects, math::float4x4 selectedObjectTransform, math::float4& debugDrawWorldPos = math::float4(1.0f));

		//AABB box
		static bool intersectsWithBox(math::float3 rayOrigin, math::float3 rayDir, float& dist, dm::box3 boundingBox);

		static bool intersectWithTriangle(math::float3 rayOrigin, math::float3 rayDir, math::float3 position0, math::float3 position1, math::float3 position2, float& dist);
	};
}