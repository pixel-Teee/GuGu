#include <pch.h>
#include "CategoryPropertyNode.h"
#include "ItemPropertyNode.h"
#include <Core/Reflection/ReflectionDatabase.h>

namespace GuGu {

	CategoryPropertyNode::CategoryPropertyNode()
	{

	}

	CategoryPropertyNode::~CategoryPropertyNode()
	{

	}

	void CategoryPropertyNode::initChildNodes()
	{
		std::vector<meta::Field*> properties;

		ComplexPropertyNode* complexNode = findComplexParent();
		if (complexNode)
		{
			ObjectPropertyNode* objectNode = findObjectItemParent();

			//const std::vector<meta::Type>& structures = complexNode->getAllStructures();

			std::vector<meta::Type> structures;
			structures.push_back(meta::Type::GetFromName(getCategoryName()));

			for (const meta::Type& type : structures)
			{
				//iterate field
				auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;
				for (auto& field : fields)
				{
					properties.push_back(&field);
				}
			}
		}

		for (int32_t propertyIndex = 0; propertyIndex < properties.size(); ++propertyIndex)
		{
			std::shared_ptr<ItemPropertyNode> newItemNode = std::make_shared<ItemPropertyNode>();

			PropertyNodeInitParams initParams;
			initParams.m_parentNode = shared_from_this();
			initParams.m_property = properties[propertyIndex];

			newItemNode->initNode(initParams);

			addChildNode(newItemNode);
		}
	}

}