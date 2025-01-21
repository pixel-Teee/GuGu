#pragma once

#include "StaticMesh.h"

namespace GuGu {
	struct GeometryHelper
	{
		//slice count:圆柱体的横截面切片数量
		//stack count:圆柱体的层数
		GStaticMesh createCylinder(float bottomRaidus, float topRadius, float height,
		uint32_t sliceCount, uint32_t stackCount);

		void createCylinderTopCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount);

		void createCylinderBottomCap(GStaticMesh& staticMesh, float bottomRaidus, float topRadius, float height,
			uint32_t sliceCount, uint32_t stackCount);
	};
}