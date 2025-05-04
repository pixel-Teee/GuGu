#pragma once

#include "MathStructCustomization.h"

namespace GuGu {
	//三分量
	class VectorStructCustomization : public MathStructCustomization
	{
	public:
		static std::shared_ptr<VectorStructCustomization> create();
	};
}