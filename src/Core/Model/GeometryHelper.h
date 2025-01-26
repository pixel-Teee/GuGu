#pragma once

#include "StaticMesh.h"

namespace GuGu {
	struct GeometryHelper
	{
		//slice count:圆柱体的横截面切片数量
		//stack count:圆柱体的层数
		static GStaticMesh createCylinder(float bottomRaidus, float topRadius, float height,
		uint32_t sliceCount, uint32_t stackCount);

		static void createCylinderTopCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount);

		static void createCylinderBottomCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount);

		static GStaticMesh createQuat(float x, float y, float w, float h, float depth);

		static GStaticMesh createGrid(float width, float depth, uint32_t m, uint32_t n);
	};
}