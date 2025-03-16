#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class PropertyNode;
	class ComplexPropertyNode;
	namespace DetailLayoutHelpers
	{
		void updateSinglePropertyMapRecursive(PropertyNode& inNode, GuGuUtf8Str curCategory, ComplexPropertyNode* curObjectNode);
	}
}