#include <pch.h>

#include "Collision3D.h"
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/GamePlay/StaticMeshComponent.h>
#include <Core/GamePlay/TerrainComponent.h>
#include "Collision3D.h"

namespace GuGu {
	std::shared_ptr<GameObject> Collision3D::pick(uint32_t x, uint32_t y,
						   uint32_t clientWidth, uint32_t clientHeight, 
						   math::float4x4 perspectiveMatrix, math::float4x4 viewMatrix, 
						   const std::vector<std::shared_ptr<GameObject>>& objects, math::float4& debugDrawWorldPos)
	{
		//观察空间中的摄像
		float vx = (2.0f * x / clientWidth - 1.0f) / perspectiveMatrix[0][0];
		float vy = (-2.0f * y / clientHeight + 1.0f) / perspectiveMatrix[1][1];

		//GuGu_LOGD("{%f, %f}", vx, vy);

		math::float4 rayOrigin = math::float4(0.0f, 0.0f, 0.0f, 1.0f);
		math::float4 rayDir = math::float4(vx, vy, 1.0f, 0.0f);

		math::float4x4 invView = math::inverse(viewMatrix);
		math::float4 worldRayOrigin = rayOrigin * invView;
		math::float4 worldRayDir = math::normalize(rayDir * invView);

		//debug draw
		//debugDrawWorldPos = worldRayOrigin + worldRayDir;

		//GuGu_LOGD("(%f %f %f), (%f %f %f)", worldRayOrigin.x, worldRayOrigin.y, worldRayOrigin.z, worldRayDir.x, worldRayDir.y, worldRayDir.z);

		std::shared_ptr<GameObject> pickedGameObject;
		for (const auto& item : objects)
		{			
			const std::shared_ptr<TransformComponent>& transformComponent = item->getComponent<TransformComponent>();
			const std::shared_ptr<StaticMeshComponent>& staticMeshComponent = item->getComponent<StaticMeshComponent>();
			//const std::shared_ptr<TerrainComponent>& terrainComponent = item->getComponent<TerrainComponent>();
			if (transformComponent && staticMeshComponent)
			{
				math::float4x4 invView = math::inverse(viewMatrix);
				math::float4x4 invWorld = math::float4x4(math::inverse(math::affineToHomogeneous(transformComponent->GetLocalToWorldTransform())));
			
				math::float4x4 toLocal = invView * invWorld;

				math::float4 localRayOrigin = rayOrigin * toLocal;
				math::float4 localRayDir = math::normalize(rayDir * toLocal);
				
				math::box3 boundingBox = staticMeshComponent->getStaticMesh()->getObjectSpaceBounds();

				float tmin = 0.0f;
				if (intersectsWithBox(localRayOrigin, localRayDir, tmin, boundingBox))
				{
					const auto& positions = staticMeshComponent->getStaticMesh()->m_positionData;
					const auto& indices = staticMeshComponent->getStaticMesh()->m_indexData;
					uint32_t triCount = indices.size() / 3;
					tmin = std::numeric_limits<float>::infinity();
					for (uint32_t i = 0; i < triCount; ++i)
					{
						uint32_t i0 = indices[i * 3 + 0];
						uint32_t i1 = indices[i * 3 + 1];
						uint32_t i2 = indices[i * 3 + 2];

						math::float3 position0 = positions[i0];
						math::float3 position1 = positions[i1];
						math::float3 position2 = positions[i2];

						float t = 0.0f;
						if (intersectWithTriangle(localRayOrigin, localRayDir, position0, position1, position2, t))
						{
							if (t < tmin)
							{
								tmin = t;
								pickedGameObject = item;
							}
						}
					}
				}
			}		

			//if (transformComponent && terrainComponent)
			//{
			//	math::float4x4 invView = math::inverse(viewMatrix);
			//	math::float4x4 invWorld = math::float4x4(math::inverse(math::affineToHomogeneous(transformComponent->GetLocalToWorldTransform())));
			//
			//	math::float4x4 toLocal = invView * invWorld;
			//
			//	math::float4 localRayOrigin = rayOrigin * toLocal;
			//	math::float4 localRayDir = math::normalize(rayDir * toLocal);
			//
			//	math::box3 boundingBox = staticMeshComponent->getStaticMesh()->getObjectSpaceBounds();
			//
			//	float tmin = 0.0f;
			//	if (intersectsWithBox(localRayOrigin, localRayDir, tmin, boundingBox))
			//	{
			//		const auto& positions = staticMeshComponent->getStaticMesh()->m_positionData;
			//		const auto& indices = staticMeshComponent->getStaticMesh()->m_indexData;
			//		uint32_t triCount = indices.size() / 3;
			//		tmin = std::numeric_limits<float>::infinity();
			//		for (uint32_t i = 0; i < triCount; ++i)
			//		{
			//			uint32_t i0 = indices[i * 3 + 0];
			//			uint32_t i1 = indices[i * 3 + 1];
			//			uint32_t i2 = indices[i * 3 + 2];
			//
			//			math::float3 position0 = positions[i0];
			//			math::float3 position1 = positions[i1];
			//			math::float3 position2 = positions[i2];
			//
			//			float t = 0.0f;
			//			if (intersectWithTriangle(localRayOrigin, localRayDir, position0, position1, position2, t))
			//			{
			//				if (t < tmin)
			//				{
			//					tmin = t;
			//					pickedGameObject = item;
			//				}
			//			}
			//		}
			//	}
			//}
		}

		return pickedGameObject;
	}

	std::shared_ptr<GuGu::GStaticMesh> Collision3D::pick(uint32_t x, uint32_t y, 
		uint32_t clientWidth, uint32_t clientHeight, 
		math::float4x4 perspectiveMatrix, math::float4x4 viewMatrix, 
		const std::vector<std::shared_ptr<GStaticMesh>>& objects, math::float4x4 selectedObjectTransform, math::float4& debugDrawWorldPos /*= math::float4(1.0f)*/)
	{
		//观察空间中的摄像
		float vx = (2.0f * x / clientWidth - 1.0f) / perspectiveMatrix[0][0];
		float vy = (-2.0f * y / clientHeight + 1.0f) / perspectiveMatrix[1][1];

		//GuGu_LOGD("{%f, %f}", vx, vy);

		math::float4 rayOrigin = math::float4(0.0f, 0.0f, 0.0f, 1.0f);
		math::float4 rayDir = math::float4(vx, vy, 1.0f, 0.0f);

		math::float4x4 invView = math::inverse(viewMatrix);
		math::float4 worldRayOrigin = rayOrigin * invView;
		math::float4 worldRayDir = math::normalize(rayDir * invView);

		//debug draw
		//debugDrawWorldPos = worldRayOrigin + worldRayDir;

		//GuGu_LOGD("(%f %f %f), (%f %f %f)", worldRayOrigin.x, worldRayOrigin.y, worldRayOrigin.z, worldRayDir.x, worldRayDir.y, worldRayDir.z);

		std::shared_ptr<GStaticMesh> pickedGameObject;
		for (const auto& item : objects)
		{

			math::float4x4 invView = math::inverse(viewMatrix);
			math::float4x4 invWorld = math::inverse(selectedObjectTransform);

			math::float4x4 toLocal = invView * invWorld;

			math::float4 localRayOrigin = rayOrigin * toLocal;
			math::float4 localRayDir = math::normalize(rayDir * toLocal);

			math::box3 boundingBox = item->getObjectSpaceBounds();

			float tmin = 0.0f;
			if (intersectsWithBox(localRayOrigin, localRayDir, tmin, boundingBox))
			{
				const auto& positions = item->m_positionData;
				const auto& indices = item->m_indexData;
				uint32_t triCount = indices.size() / 3;
				tmin = std::numeric_limits<float>::infinity();
				for (uint32_t i = 0; i < triCount; ++i)
				{
					uint32_t i0 = indices[i * 3 + 0];
					uint32_t i1 = indices[i * 3 + 1];
					uint32_t i2 = indices[i * 3 + 2];

					math::float3 position0 = positions[i0];
					math::float3 position1 = positions[i1];
					math::float3 position2 = positions[i2];

					float t = 0.0f;
					if (intersectWithTriangle(localRayOrigin, localRayDir, position0, position1, position2, t))
					{
						if (t < tmin)
						{
							tmin = t;
							pickedGameObject = item;
						}
					}
				}
			}
		}

		return pickedGameObject;
	}

	bool Collision3D::intersectsWithPlane(math::float3 rayOrigin, math::float3 rayDir, math::float3 planeNormal, math::float3 planePoint, math::float3& res)
	{
		float t = math::dot(planeNormal, (-rayOrigin + planePoint)) / math::dot(planeNormal, rayDir);

		if (t <= 0)
		{
			return false;
		}
		else
		{
			res = rayOrigin + t * rayDir;
			return true;
		}
	}

	void Collision3D::calculateRayOriginAndRayDir(uint32_t x, uint32_t y, uint32_t clientWidth, uint32_t clientHeight, math::float4x4 perspectiveMatrix, math::float4x4 viewMatrix, math::float3& outRayOrigin, math::float3& outRayDir)
	{
		//观察空间中的摄像
		float vx = (2.0f * x / clientWidth - 1.0f) / perspectiveMatrix[0][0];
		float vy = (-2.0f * y / clientHeight + 1.0f) / perspectiveMatrix[1][1];

		//GuGu_LOGD("{%f, %f}", vx, vy);

		math::float4 rayOrigin = math::float4(0.0f, 0.0f, 0.0f, 1.0f);
		math::float4 rayDir = math::float4(vx, vy, 1.0f, 0.0f);

		math::float4x4 invView = math::inverse(viewMatrix);
		outRayOrigin = rayOrigin * invView;
		outRayDir = math::normalize(rayDir * invView);
	}

	bool Collision3D::intersectsWithBox(math::float3 rayOrigin, math::float3 rayDir, float& dist, dm::box3 boundingBox)
	{
		math::float3 boundingMin = boundingBox.m_mins, boundingMax = boundingBox.m_maxs;

		//计算射线与AABB各平面的交点
		float tmin = -std::numeric_limits<float>::infinity();
		float tmax = std::numeric_limits<float>::infinity();

		//x 轴方向
		if (rayDir.x != 0)
		{
			float t1 = (boundingMin.x - rayOrigin.x) / rayDir.x;
			float t2 = (boundingMax.x - rayOrigin.x) / rayDir.x;
			if (t1 > t2) std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
		}

		if (rayDir.y != 0)
		{
			float t1 = (boundingMin.y - rayOrigin.y) / rayDir.y;
			float t2 = (boundingMax.y - rayOrigin.y) / rayDir.y;
			if (t1 > t2) std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
		}

		if (rayDir.z != 0)
		{
			float t1 = (boundingMin.z - rayOrigin.z) / rayDir.z;
			float t2 = (boundingMax.z - rayOrigin.z) / rayDir.z;
			if (t1 > t2) std::swap(t1, t2);
			tmin = std::max(tmin, t1);
			tmax = std::min(tmax, t2);
		}

		if (tmin > tmax) return false;

		dist = tmin;
		if (dist < 0) dist = tmax;
		return dist >= 0;
	}
	bool Collision3D::intersectWithTriangle(math::float3 rayOrigin, math::float3 rayDir, math::float3 position0, math::float3 position1, math::float3 position2, float& dist)
	{
		math::float3 e1 = position1 - position0;
		math::float3 e2 = position2 - position0;

		math::float3 p = math::cross(rayDir, e2);
		
		double det = math::dot(e1, p);

		if (det > -math::epsilon && det < math::epsilon)
		{
			return false;
		}
		double invDet = 1.0f / det;

		math::float3 T = rayOrigin - position0;

		double u = math::dot(T, p) * invDet;

		if (u < 0.0f || u > 1.0f)
		{
			return false;
		}

		math::float3 q = math::cross(T, e1);

		double v = math::dot(rayDir, q) * invDet;

		if (v < 0.0f || u + v > 1.0f)
		{
			return false;
		}

		double t = math::dot(e2, q) * invDet;

		if (t > math::epsilon)
		{
			dist = t;
			return true;
		}

		return false;
	}
}