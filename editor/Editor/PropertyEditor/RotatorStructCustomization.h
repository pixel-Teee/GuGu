#pragma once

#include "MathStructCustomization.h"

namespace GuGu {
	class RotatorStructCustomization : public MathStructCustomization
	{
	public:
		static std::shared_ptr<RotatorStructCustomization> create();
	};
}