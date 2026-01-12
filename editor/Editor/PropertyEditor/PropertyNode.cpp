#include <pch.h>

#include "PropertyNode.h"
#include <Core/Reflection/ReflectionDatabase.h>

namespace GuGu {

	PropertyNode::PropertyNode()
	{

	}

	PropertyNode::~PropertyNode()
	{

	}

	void PropertyNode::initNode(const PropertyNodeInitParams& initParams)
	{
		destoryTree();

		m_parentNode = initParams.m_parentNode.get();
		m_parentNodeWeakPtr = initParams.m_parentNode;

		//property data
		m_property = initParams.m_property;

		initBeforeNodeFlags();

		rebuildChildren();
	}

	void PropertyNode::rebuildChildren()
	{
		destoryTree();

		if (m_childNodes.size() == 0)
		{
			initChildNodes();
		}
	}

	std::shared_ptr<PropertyNode> PropertyNode::findChildPropertyNode(const GuGuUtf8Str& inPropertyName, bool bRecurse /*= false*/)
	{
		for (const std::shared_ptr<PropertyNode>& childNode : m_childNodes)
		{
			if (childNode->getField() && childNode->getField()->GetName() == inPropertyName)
			{
				return childNode;
			}
		}

		if (bRecurse)
		{
			for (const std::shared_ptr<PropertyNode>& childNode : m_childNodes)
			{
				std::shared_ptr<PropertyNode> propertyNode = childNode->findChildPropertyNode(inPropertyName, bRecurse);
				if (propertyNode)
				{
					return propertyNode;
				}
			}
		}
		return nullptr;
	}

	void PropertyNode::addChildNode(std::shared_ptr<PropertyNode> inNode)
	{
		m_childNodes.push_back(inNode);
	}

	ComplexPropertyNode* PropertyNode::findComplexParent()
	{
		PropertyNode* cur = this;
		ComplexPropertyNode* found = nullptr;
		while (true)
		{
			found = cur->asComplexNode();
			if (found)
			{
				break;
			}
			cur = cur->getParentNode();
			if (!cur)
			{
				break;
			}
		}
		return found;
	}

	ObjectPropertyNode* PropertyNode::findObjectItemParent()
	{
		ComplexPropertyNode* complexParent = findComplexParent();
		if (!complexParent)
		{
			return nullptr;
		}
		if (ObjectPropertyNode* objectNode = complexParent->asObjectNode())
		{
			return objectNode;
		}
		else if (PropertyNode* parentNodePtr = complexParent->getParentNode())
		{
			return parentNodePtr->findObjectItemParent();
		}
		return nullptr;
	}

	GuGu::meta::Variant PropertyNode::getOwnerFieldVariant(const meta::Variant& startVarint)
	{
		if (m_parentNodeWeakPtr.lock())
		{
			return m_parentNodeWeakPtr.lock()->getOwnerFieldVariant(startVarint);
		}
		return meta::Variant();//nothing
	}

	PropertyAccess::Result PropertyNode::getPropertyValueString(GuGuUtf8Str& outString)
	{
		PropertyAccess::Result result = PropertyAccess::Result::Fail;

		meta::Type curType = getField()->GetType();
		if (curType.IsValid())
		{
			auto& fields = meta::ReflectionDatabase::Instance().types[curType.GetID()].fields;

			std::vector<meta::Object*> objectsToRead;
			ComplexPropertyNode* complexNode = findComplexParent();
			if (complexNode)
			{
				const int32_t numObjects = complexNode->getInstancesNum();
				for (int32_t index = 0; index < numObjects; ++index)
				{
					objectsToRead.push_back(complexNode->getInstanceAsObject(index));
				}
			}

			//获取当前字段所在结构体的variant
			std::vector<meta::Variant> owners;
			for (int32_t i = 0; i < objectsToRead.size(); ++i)
			{
				meta::Variant startVarint = ObjectVariant(objectsToRead[i]);
				meta::Variant owner = getOwnerFieldVariant(startVarint);
				if (owner != meta::Variant())
					owners.push_back(owner);
			}
			if (curType == typeof(GuGuUtf8Str))
			{
				meta::Field checkField = meta::ReflectionDatabase::Instance().types[owners[0].GetType().GetID()].GetField(getField()->GetName().getStr());//have this field?
				if (checkField.GetType() == getField()->GetType() && checkField.GetName() == getField()->GetName())
				{
					outString = checkField.GetValue(owners[0]).ToString();
					return PropertyAccess::Success;
				}
			}
			if (curType.IsEnum())
			{
				//enum just return str
				meta::Field checkField = meta::ReflectionDatabase::Instance().types[owners[0].GetType().GetID()].GetField(getField()->GetName().getStr());//have this field?
				if (checkField.GetType() == getField()->GetType() && checkField.GetName() == getField()->GetName())
				{
					outString = checkField.GetType().GetEnum().GetKey(checkField.GetValue(owners[0]));
					return PropertyAccess::Success;
				}
			}
			if (owners.size() > 0)
			{
				const meta::Variant& instance = getField()->GetValue(owners[0]);
				outString += "(";
				for (int32_t i = 0; i < fields.size(); ++i)
				{
					meta::Field curField = meta::ReflectionDatabase::Instance().types[instance.GetType().GetID()].GetField(fields[i].GetName().getStr());//have this field?
					if ((curField.GetType() == fields[i].GetType()) && (curField.GetName() == fields[i].GetName()))
					{
						//meta::Variant& instance = owners[0];
						meta::Variant fieldValue = fields[i].GetValue(instance);

						outString += curField.GetName() + " = ";
						outString += fieldValue.ToString();
						if(i != fields.size() - 1)
							outString += ", ";
					}
				}
				outString += ")";
			}
			
			result = PropertyAccess::Success;
		}

		return result;
	}

	void PropertyNode::destoryTree()
	{
		m_childNodes.clear();
	}

}