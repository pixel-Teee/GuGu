#pragma once

#include "MathStructCustomization.h"

namespace GuGu {
	class Vector4StructCustomization : public MathStructCustomization
	{
	public:
		static std::shared_ptr<Vector4StructCustomization> create();
	};
}