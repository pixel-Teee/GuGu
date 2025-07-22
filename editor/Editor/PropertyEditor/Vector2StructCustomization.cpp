#include <pch.h>

#include "Vector2StructCustomization.h"

namespace GuGu {

	std::shared_ptr<Vector2StructCustomization> Vector2StructCustomization::create()
	{
		return std::make_shared<Vector2StructCustomization>();
	}

}