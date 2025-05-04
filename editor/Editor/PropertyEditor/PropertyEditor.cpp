#include <pch.h>

#include "PropertyEditor.h"
#include "ItemPropertyNode.h"
#include "PropertyHandleImp.h"//PropertyHandleFloat

namespace GuGu {

	PropertyEditor::PropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode)
		: m_propertyNode(inPropertyNode)
	{
		if (PropertyHandleFloat::supports(inPropertyNode))
		{
			m_propertyHandle = std::make_shared<PropertyHandleFloat>(inPropertyNode);
		}
		else if (PropertyHandleDouble::supports(inPropertyNode))
		{
			m_propertyHandle = std::make_shared<PropertyHandleDouble>(inPropertyNode);
		}
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

	std::shared_ptr<IPropertyHandle> PropertyEditor::getPropertyHandle() const
	{
		return m_propertyHandle;
	}

	const meta::Field* PropertyEditor::getField() const
	{
		return m_propertyNode->getField();
	}

}