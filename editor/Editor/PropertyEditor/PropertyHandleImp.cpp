#include <pch.h>

#include "PropertyHandleImp.h"
#include "PropertyNode.h"
#include "ObjectPropertyNode.h"
#include <Core/Reflection/Field.h>
#include <Core/Reflection/Object.h>
#include <Core/Reflection/Variant.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/Reflection/ReflectionDatabase.h>

namespace GuGu {

	PropertyValueImpl::PropertyValueImpl(std::shared_ptr<PropertyNode> inPropertyNode)
		: m_propertyNode(inPropertyNode)
	{

	}

	PropertyAccess::Result PropertyValueImpl::getValueData(meta::Variant& fieldValue)
	{
		std::vector<meta::Object*> objectsToRead;
		std::shared_ptr<PropertyNode> propertyNodeLock = m_propertyNode.lock();
		ComplexPropertyNode* complexNode = propertyNodeLock->findComplexParent();
		if (complexNode)
		{
			const int32_t numObjects = complexNode->getInstancesNum();
			for (int32_t index = 0; index < numObjects; ++index)
			{
				objectsToRead.push_back(complexNode->getInstanceAsObject(index));
			}
		}

		PropertyAccess::Result result = PropertyAccess::Success;
		//modify
		meta::Field* field = propertyNodeLock->getField();
		for (int32_t i = 0; i < objectsToRead.size(); ++i)
		{
			meta::Field curField = meta::ReflectionDatabase::Instance().types[objectsToRead[i]->GetType().GetID()].GetField(field->GetName().getStr());//have this field?

			meta::Variant instance = ObjectVariant(objectsToRead[i]);
			if (curField.GetType() == field->GetType())
			{
				fieldValue = propertyNodeLock->getField()->GetValue(instance);
			}
			//meta::Variant fieldValue;	
		}

		return result;
	}

	PropertyAccess::Result PropertyValueImpl::importText(const GuGuUtf8Str& inValue)
	{
		std::shared_ptr<PropertyNode> propertyNodeLock = m_propertyNode.lock();
		if (propertyNodeLock)
		{
			
			return importText(inValue, propertyNodeLock.get());
		}
		return PropertyAccess::Fail;
	}

	PropertyAccess::Result PropertyValueImpl::importText(const GuGuUtf8Str& inValue, PropertyNode* inPropertyNode)
	{
		std::vector<meta::Object*> objectsToModify;
		ComplexPropertyNode* complexNode = inPropertyNode->findComplexParent();
		if (complexNode)
		{
			const int32_t numObjects = complexNode->getInstancesNum();
			for (int32_t index = 0; index < numObjects; ++index)
			{
				objectsToModify.push_back(complexNode->getInstanceAsObject(index));
			}
		}

		//获取当前字段所在结构体的variant
		std::vector<meta::Variant> owners;
		for (int32_t i = 0; i < objectsToModify.size(); ++i)
		{
			meta::Variant startVarint = ObjectVariant(objectsToModify[i]);
			meta::Variant owner = inPropertyNode->getOwnerFieldVarint(startVarint);
			if(owner != meta::Variant())
				owners.push_back(owner);
		}

		std::string actualStr = inValue.getStr();
		float actualValue = std::stof(actualStr);
		PropertyAccess::Result result = PropertyAccess::Success;
		//modify
		meta::Field* field = inPropertyNode->getField();
		for (int32_t i = 0; i < owners.size(); ++i)
		{
			meta::Field curField = meta::ReflectionDatabase::Instance().types[owners[i].GetType().GetID()].GetField(field->GetName().getStr());//have this field?
			if (curField.GetType() == field->GetType())
			{
				meta::Variant instance = owners[i];
				meta::Variant fieldValue(actualValue);
				inPropertyNode->getField()->SetValue(instance, fieldValue);
			}
		}
		
		return result;
	}

#define IMPLEMENT_PROPERTY_ACCESSOR(ValueType)\
	PropertyAccess::Result PropertyHandleBase::setValue(ValueType const& inValue)\
	{\
		return PropertyAccess::Fail;\
	}\
	PropertyAccess::Result PropertyHandleBase::getValue(ValueType& outValue) const\
	{\
		return PropertyAccess::Fail;\
	}\

	IMPLEMENT_PROPERTY_ACCESSOR(float)

	PropertyHandleBase::PropertyHandleBase(std::shared_ptr<PropertyNode> propertyNode)
		: m_implementation(std::make_shared<PropertyValueImpl>(propertyNode))
	{

	}

#define IMPLEMENT_PROPERTY_VALUE(ClassName)\
	ClassName::ClassName(std::shared_ptr<PropertyNode> propertyNode) \
	: PropertyHandleBase(propertyNode)\
	{}

	IMPLEMENT_PROPERTY_VALUE(PropertyHandleFloat)

	bool PropertyHandleFloat::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if(field == nullptr)
		{
			return false;
		}
		return field->GetType().IsFloatingPoint();//todo:修复这个
	}

	PropertyAccess::Result PropertyHandleFloat::getValue(float& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToFloat();

		return res;
	}

	PropertyAccess::Result PropertyHandleFloat::setValue(const float& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%f", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

}