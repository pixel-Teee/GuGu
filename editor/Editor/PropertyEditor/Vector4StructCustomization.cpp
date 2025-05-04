#include <pch.h>

#include "Vector4StructCustomization.h"

namespace GuGu {

	std::shared_ptr<Vector4StructCustomization> Vector4StructCustomization::create()
	{
		return std::make_shared<Vector4StructCustomization>();
	}

}