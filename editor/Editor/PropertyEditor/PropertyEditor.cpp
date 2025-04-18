#include <pch.h>

#include "PropertyEditor.h"
#include "ItemPropertyNode.h"

namespace GuGu {

	PropertyEditor::PropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode)
		: m_propertyNode(inPropertyNode)
	{

	}

	GuGuUtf8Str PropertyEditor::getDisplayName() const
	{
		ItemPropertyNode* itemPropertyNode = m_propertyNode->asItemPropertyNode();

		if (itemPropertyNode != nullptr)
		{
			return itemPropertyNode->getDisplayName();
		}
		else
		{
			//
		}

		return "";
	}

	std::shared_ptr<PropertyNode> PropertyEditor::getPropertyNode() const
	{
		return m_propertyNode;
	}

	const meta::Field* PropertyEditor::getField() const
	{
		return m_propertyNode->getField();
	}

}