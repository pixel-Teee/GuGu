#include <pch.h>

#include "ItemPropertyNode.h"
#include <Core/Reflection/ReflectionDatabase.h>

namespace GuGu {

	ItemPropertyNode::ItemPropertyNode()
	{

	}

	ItemPropertyNode::~ItemPropertyNode()
	{

	}

	GuGuUtf8Str ItemPropertyNode::getDisplayName() const
	{
		return m_property->GetName();
	}

	meta::Variant ItemPropertyNode::getOwnerFieldVarint(const meta::Variant& startVarint)
	{
		meta::Variant result;
		meta::Field* field = getField();
		meta::Type parentType = field->GetClassType();
		if (m_parentNodeWeakPtr.lock())
		{
			meta::Variant parentVarint = m_parentNodeWeakPtr.lock()->getOwnerFieldVarint(startVarint);
			//has this field?
			std::vector<meta::Field> checkField = meta::ReflectionDatabase::Instance().types[parentVarint.GetType().GetID()].fields;
			bool haveThisField = false;
			for (size_t i = 0; i < checkField.size(); ++i)
			{
				if (checkField[i].GetType() == field->GetType())
					haveThisField = true;
			}

			if(parentVarint != meta::Variant() && haveThisField)
				result = field->GetValue(parentVarint);
		}
		return result;
	}

	void ItemPropertyNode::initChildNodes()
	{
		meta::Field* field = getField();
		if (field->GetType().IsClass())//class
		{
			auto& fields = meta::ReflectionDatabase::Instance().types[field->GetType().GetID()].fields;
			for (auto& field : fields)
			{
				std::shared_ptr<ItemPropertyNode> newItemNode = std::make_shared<ItemPropertyNode>();
				PropertyNodeInitParams initParams;
				initParams.m_parentNode = shared_from_this();
				initParams.m_property = &field;

				newItemNode->initNode(initParams);
				addChildNode(newItemNode);
			}
		}
	}

}