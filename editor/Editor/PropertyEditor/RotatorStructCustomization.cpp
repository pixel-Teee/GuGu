#include <pch.h>

#include "RotatorStructCustomization.h"

namespace GuGu {

	std::shared_ptr<RotatorStructCustomization> RotatorStructCustomization::create()
	{
		return std::make_shared<RotatorStructCustomization>();
	}

}