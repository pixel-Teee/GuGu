#include <pch.h>
#include "VectorStructCustomization.h"

namespace GuGu {

	std::shared_ptr<VectorStructCustomization> VectorStructCustomization::create()
	{
		return std::make_shared<VectorStructCustomization>();
	}

}