#pragma once

#include "MathStructCustomization.h"

namespace GuGu {
	//二分量
	class Vector2StructCustomization : public MathStructCustomization
	{
	public:
		static std::shared_ptr<Vector2StructCustomization> create();
	};
}