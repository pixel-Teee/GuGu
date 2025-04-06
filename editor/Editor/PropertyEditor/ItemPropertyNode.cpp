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