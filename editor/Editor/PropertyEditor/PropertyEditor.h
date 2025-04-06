#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class PropertyNode;
	class PropertyEditor
	{
	public:
		PropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode);

		GuGuUtf8Str getDisplayName() const;
	private:
		std::shared_ptr<PropertyNode> m_propertyNode;
	};
}