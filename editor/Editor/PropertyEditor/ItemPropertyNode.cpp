#include <pch.h>

#include "ItemPropertyNode.h"
#include "ObjectPropertyNode.h"
#include "CategoryPropertyNode.h"
#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/AssetManager/AssetData.h>

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

	GuGu::meta::Variant ItemPropertyNode::getOwnerFieldVariant(const meta::Variant& startVariant)
	{
		meta::Variant result;
		meta::Field* field = getField();
		meta::Type parentType = field->GetClassType();//字段所在的类
		//先检查自己
		std::vector<meta::Field> checkFields = meta::ReflectionDatabase::Instance().types[startVariant.GetType().GetID()].fields;
		bool haveThisField = false;
		for (size_t i = 0; i < checkFields.size(); ++i)
		{
			if ((checkFields[i].GetType() == field->GetType()) && (checkFields[i].GetName() == field->GetName()))
				haveThisField = true;
		}
		if(haveThisField)
			return startVariant;

		//bool typeIsEqualStartVariant = false;
		meta::Type currentType = parentType;
		PropertyNode* currentPropertyNode = m_parentNode;
		meta::Type startVariantType = startVariant.GetType();
		while (currentType != startVariantType)
		{
			if (currentPropertyNode == nullptr)
				break;
			if (currentPropertyNode->getField() != nullptr)
				currentType = currentPropertyNode->getField()->GetType();
			if (currentPropertyNode->asCategoryNode() != nullptr)
			{
				CategoryPropertyNode* categoryPropertyNode = currentPropertyNode->asCategoryNode();
				if (categoryPropertyNode)
				{
					currentType = meta::Type::GetFromName(categoryPropertyNode->getCategoryName());
				}
			}
			currentPropertyNode = currentPropertyNode->getParentNode();
		}
		if (currentType != startVariantType)
			return meta::Variant();

		if (m_parentNodeWeakPtr.lock())
		{
			meta::Variant parentVariant = m_parentNodeWeakPtr.lock()->getOwnerFieldVariant(startVariant);
			//has this field?
			std::vector<meta::Field> checkFields = meta::ReflectionDatabase::Instance().types[parentVariant.GetType().GetID()].fields;

			meta::Field* parentField = m_parentNodeWeakPtr.lock()->getField();
			if (parentField)
			{
				bool haveThisField = false;
				for (size_t i = 0; i < checkFields.size(); ++i)
				{
					if ((checkFields[i].GetType() == parentField->GetType()) && (checkFields[i].GetName() == parentField->GetName()))
						haveThisField = true;
				}

				if (parentVariant != meta::Variant() && haveThisField)
					result = parentField->GetValueReference(parentVariant);
			}	
		}
		return result;
	}

	void ItemPropertyNode::initChildNodes()
	{
		meta::Field* field = getField();
		if (field->GetType().IsWeakPtr()) //hack
			return;
		if (field->GetType().IsStruct() || field->GetType() == typeof(std::shared_ptr<AssetData>)
			|| field->GetType() == typeof(GuGuUtf8Str)) //struct
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
		else if (field->GetType().IsClass())//class
		{
			ObjectPropertyNode* parentObjectNode = findObjectItemParent();

			std::shared_ptr<ObjectPropertyNode> newObjectNode = std::make_shared<ObjectPropertyNode>();
			for (int32_t i = 0; i < parentObjectNode->getNumObjects(); ++i)
			{
				meta::Object* object = parentObjectNode->getObject(i);
				meta::Variant instance = ObjectVariant(object);

				//find field
				std::vector<meta::Field> checkFields = meta::ReflectionDatabase::Instance().types[instance.GetType().GetID()].fields;
				bool haveThisField = false;
				for (size_t i = 0; i < checkFields.size(); ++i)
				{
					if ((checkFields[i].GetType() == field->GetType()) && (checkFields[i].GetName() == field->GetName()))
						haveThisField = true;
				}
				if (haveThisField)
				{
					meta::Variant var = field->GetValue(instance);
					std::shared_ptr<meta::Object> fieldObject = var.GetValue<std::shared_ptr<meta::Object>>();//获取字段对应的object

					newObjectNode->addObject(fieldObject.get());
				}	
			}

			PropertyNodeInitParams initParams;
			initParams.m_parentNode = shared_from_this();
			initParams.m_property = field;

			newObjectNode->initNode(initParams);
			addChildNode(newObjectNode);
		}
	}

}