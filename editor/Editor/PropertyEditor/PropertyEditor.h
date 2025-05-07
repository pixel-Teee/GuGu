#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Field.h>

namespace GuGu {
	class PropertyNode;
	class IPropertyHandle;
	class PropertyEditor
	{
	public:
		PropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode);

		GuGuUtf8Str getDisplayName() const;

		std::shared_ptr<PropertyNode> getPropertyNode() const;

		//get property handle
		std::shared_ptr<IPropertyHandle> getPropertyHandle() const;

		meta::Field* getField() const;
	private:
		std::shared_ptr<PropertyNode> m_propertyNode;

		std::shared_ptr<IPropertyHandle> m_propertyHandle;
	};
}