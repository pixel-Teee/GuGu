#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class PropertyNode;
	class ComplexPropertyNode;
	struct DetailLayoutData;
	namespace DetailLayoutHelpers
	{
		struct UpdatePropertyMapArgs
		{
			DetailLayoutData* m_layoutData;
		};

		void updateSinglePropertyMapRecursive(PropertyNode& inNode, GuGuUtf8Str curCategory, ComplexPropertyNode* curObjectNode, UpdatePropertyMapArgs& inUpdateArgs);
	}
}