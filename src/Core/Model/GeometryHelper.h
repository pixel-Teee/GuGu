#pragma once

#include "StaticMesh.h"

namespace GuGu {
	struct GeometryHelper
	{
		//slice count:圆柱体的横截面切片数量
		//stack count:圆柱体的层数
		static GStaticMesh createCylinder(float bottomRaidus, float topRadius, float height,
		uint32_t sliceCount, uint32_t stackCount, math::float4x4 transform = math::float4x4::identity());

		static void createCylinderTopCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount, math::float4x4 transform = math::float4x4::identity());

		static void createCylinderBottomCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount, math::float4x4 transform = math::float4x4::identity());

		static GStaticMesh createQuat(float x, float y, float w, float h, float depth);

		static GStaticMesh createGrid(float width, float depth, uint32_t m, uint32_t n);

		//圆环
		//内圈半径
		//截面半径
		//圆环的内圈有多少段
		//圆环的截面有多少段
		static GStaticMesh createToru(float R, float r, int32_t majorSegments, int32_t minorSegments, math::float4x4 transform = math::float4x4::identity());
	};
}