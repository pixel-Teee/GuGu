#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Field.h>

namespace GuGu {
	class PropertyNode;
	class PropertyEditor
	{
	public:
		PropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode);

		GuGuUtf8Str getDisplayName() const;

		std::shared_ptr<PropertyNode> getPropertyNode() const;

		const meta::Field* getField() const;
	private:
		std::shared_ptr<PropertyNode> m_propertyNode;
	};
}