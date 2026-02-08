#include <pch.h>

#include "PropertyHandleImp.h"
#include "PropertyNode.h"
#include "ObjectPropertyNode.h"
#include "PropertyEditor.h"
#include "PropertyEditorHelps.h"
#include <Core/UI/NullWidget.h>
#include <Core/Reflection/Field.h>
#include <Core/Reflection/Object.h>
#include <Core/Reflection/Variant.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GameUI/UIAnchors.h>
#include <Core/GamePlay/GameUI/UIPadding.h>
#include <Core/AssetManager/AssetData.h>
#include <Core/AssetManager/GameObjectLevelRef.h>
#include <Core/Reflection/ReflectionDatabase.h>
#include <Core/AssetManager/AssetManager.h>
#include <Editor/Transaction/TransactionManager.h>
#include <Renderer/Color.h>

namespace GuGu {

	std::vector<GuGuUtf8Str> splitByComma(const GuGuUtf8Str& str)
	{
		std::vector<GuGuUtf8Str> pairs;
		int32_t start = 0;
		for (int32_t i = 0; i < str.len(); ++i)
		{
			if (str[i] == ",")
			{
				GuGuUtf8Str pair = str.substr(start, i - start);
				pairs.push_back(pair);

				start = i + 1;
				while (start < str.len() && str[start] == " ") ++start;
			}
		}

		GuGuUtf8Str last = str.substr(start);
		last.trim();
		if (last.len()) pairs.push_back(last);
		return pairs;
	}

	PropertyValueImpl::PropertyValueImpl(std::shared_ptr<PropertyNode> inPropertyNode)
		: m_propertyNode(inPropertyNode)
	{

	}

	PropertyAccess::Result PropertyValueImpl::getValueData(meta::Variant& fieldValue)
	{
		std::shared_ptr<PropertyNode> propertyNodeLock = m_propertyNode.lock();
		std::vector<meta::Object*> objectsToRead;
		ComplexPropertyNode* complexNode = propertyNodeLock->findComplexParent();
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
			if (propertyNodeLock != nullptr)
			{
				meta::Variant owner = propertyNodeLock->getOwnerFieldVariant(startVarint);
				if (owner != meta::Variant())
					owners.push_back(owner);
			}
		}

		PropertyAccess::Result result = PropertyAccess::Success;
		//read
		meta::Field* field = propertyNodeLock->getField();
		for (int32_t i = 0; i < owners.size(); ++i)
		{
			meta::Field curField = meta::ReflectionDatabase::Instance().types[owners[i].GetType().GetID()].GetField(field->GetName().getStr());//have this field?
			if (curField.GetType() == field->GetType() && field->GetClassType() == owners[i].GetType())
			{
				meta::Variant& instance = owners[i];
				fieldValue = propertyNodeLock->getField()->GetValue(instance);
			}
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
		//undo/redo
		TransactionManager& transactionManager = TransactionManager::getTransactionManager();
		transactionManager.beginTransaction();
		for (int32_t index = 0; index < objectsToModify.size(); ++index)
		{
			transactionManager.modifyObject(objectsToModify[index]->shared_from_this());
		}

		//获取当前字段所在结构体的variant
		std::vector<meta::Variant> owners;
		for (int32_t i = 0; i < objectsToModify.size(); ++i)
		{
			meta::Variant startVarint = ObjectVariant(objectsToModify[i]);
			if (inPropertyNode != nullptr)
			{
				meta::Variant owner = inPropertyNode->getOwnerFieldVariant(startVarint);
				if (owner != meta::Variant())
					owners.push_back(std::move(owner));
			}	
		}

		std::string actualStr = inValue.getStr();
		//float actualValue = std::stof(actualStr);
		PropertyAccess::Result result = PropertyAccess::Success;
		//modify
		meta::Field* field = inPropertyNode->getField();
		meta::Variant fieldValue;//todo:这里以后要调用反射类型的虚函数，进行不同的派发
		if (field->GetType() == typeof(float))
		{
			fieldValue = std::stof(actualStr);
		}
		else if (field->GetType() == typeof(double))
		{
			fieldValue = std::stod(actualStr);
		}
		else if (field->GetType() == typeof(Color))
		{		
			//解析字符串
			//去除两边的括号
			GuGuUtf8Str ridBracket = inValue.substr(1, inValue.len() - 2);
			//按逗号分割键值对
			std::vector<GuGuUtf8Str> pairs = splitByComma(ridBracket);
			
			//解析键值对
			Color color;
			meta::Variant colorInstance(color);
			for (const GuGuUtf8Str& pair : pairs)
			{
				int32_t eqPos = pair.find("=");
				if (eqPos == -1) continue;

				GuGuUtf8Str key = pair.substr(0, eqPos);
				key = key.trim();
				GuGuUtf8Str value = pair.substr(eqPos + 1);
				value = value.trim();

				GuGuUtf8Str propertyName = key;
				meta::Type colorType = typeof(Color);
				meta::Field field = meta::ReflectionDatabase::Instance().types[colorType.GetID()].GetField(propertyName.getStr());
				if (field.IsValid())
				{
					if (field.GetType() == typeof(float))
					{
						field.SetValue(colorInstance, std::stof(value.getStr()));
					}
					else if (field.GetType() == typeof(double))
					{
						field.SetValue(colorInstance, std::stod(value.getStr()));
					}
				}
			}
			fieldValue = std::move(colorInstance);
		}
		else if (field->GetType() == typeof(std::shared_ptr<AssetData>))
		{
			GGuid guid(inValue);

			std::shared_ptr<AssetData> assetData;
			meta::Variant assetDataInstance(AssetManager::getAssetManager().loadAsset(guid));
			fieldValue = std::move(assetDataInstance);
		}
		else if (field->GetType() == typeof(std::shared_ptr<GameObjectLevelRef>))
		{
			//level ref
			std::shared_ptr<GameObjectLevelRef> objLevelRef = std::make_shared<GameObjectLevelRef>();
			meta::Variant objLevelRefInstance = *objLevelRef;
			objLevelRef->GetType().importStr(inValue, objLevelRefInstance);
			*objLevelRef = objLevelRefInstance.GetValue<GameObjectLevelRef>();
			meta::Variant objLevelRefInstance2 = objLevelRef;
			fieldValue = std::move(objLevelRefInstance2);
		}
		else if (field->GetType() == typeof(UIAnchors))
		{
			meta::Variant anchorsInstance = UIAnchors();
			field->GetType().importStr(inValue, anchorsInstance);
			fieldValue = std::move(anchorsInstance);
		}
		else if (field->GetType() == typeof(math::float2))
		{
			meta::Variant vectorInstance = math::float2();
			field->GetType().importStr(inValue, vectorInstance);
			fieldValue = std::move(vectorInstance);
		}
		else if (field->GetType() == typeof(UIPadding))
		{
			meta::Variant offsetInstance = UIPadding();
			field->GetType().importStr(inValue, offsetInstance);
			fieldValue = std::move(offsetInstance);
		}
		else if (field->GetType() == typeof(GuGuUtf8Str))
		{
			GuGuUtf8Str str = inValue;
			meta::Variant strInstance = str;
			fieldValue = std::move(strInstance);
		}
		else if (field->GetType() == typeof(int8_t))
		{
			fieldValue = (int8_t)std::stoi(actualStr);
		}
		else if (field->GetType() == typeof(int16_t))
		{
			fieldValue = (int16_t)std::stoi(actualStr);
		}
		else if (field->GetType() == typeof(int32_t))
		{
			fieldValue = (int32_t)std::stoi(actualStr);
		}
		else if (field->GetType() == typeof(int64_t))
		{
			fieldValue = (int64_t)std::stoll(actualStr);//long long
		}
		else if (field->GetType() == typeof(uint8_t))
		{
			fieldValue = (uint8_t)std::stoi(actualStr);
		}
		else if (field->GetType() == typeof(uint16_t))
		{
			fieldValue = (uint16_t)std::stoul(actualStr);
		}
		else if (field->GetType() == typeof(uint32_t))
		{
			fieldValue = (uint32_t)std::stoul(actualStr);
		}
		else if (field->GetType() == typeof(uint64_t))
		{
			fieldValue = (uint64_t)std::stoull(actualStr);
		}
		else if (field->GetType().IsEnum())
		{
			//enum
			fieldValue = field->GetType().GetEnum().GetValue(actualStr);
		}
		for (int32_t i = 0; i < owners.size(); ++i)
		{
			meta::Field curField = meta::ReflectionDatabase::Instance().types[owners[i].GetType().GetID()].GetField(field->GetName().getStr());//have this field?
			if (curField.GetType() == field->GetType() && field->GetClassType() == owners[i].GetType())
			{
				meta::Variant& instance = owners[i];
		
				inPropertyNode->getField()->SetValue(instance, fieldValue);	
			}
		}
		transactionManager.commit();

		return result;
	}

	PropertyAccess::Result PropertyValueImpl::setValueAsString(const GuGuUtf8Str& inValue)
	{
		PropertyAccess::Result result = PropertyAccess::Fail;

		std::shared_ptr<PropertyNode> propertyNodeLocked = m_propertyNode.lock();
		if (propertyNodeLocked)
		{
			meta::Field* nodeProperty = propertyNodeLocked->getField();

			GuGuUtf8Str value = inValue;

			//todo:对字符串进行一些处理，去除空格

			importText(value, propertyNodeLocked.get());

			result = PropertyAccess::Success;
		}

		return result;
	}

	PropertyAccess::Result PropertyValueImpl::getValueAsDisplayString(GuGuUtf8Str& outString)
	{
		std::shared_ptr<PropertyNode> propertyNodeLocked = m_propertyNode.lock();

		PropertyAccess::Result res = PropertyAccess::Success;

		if (propertyNodeLocked)
		{
			res = propertyNodeLocked->getPropertyValueString(outString);
		}
		else
		{
			res = PropertyAccess::Fail;
		}

		return res;
	}

	int32_t PropertyValueImpl::getNumChildren() const
	{
		std::shared_ptr<PropertyNode> propertyNodeLock = m_propertyNode.lock();
		if (propertyNodeLock)
		{
			return propertyNodeLock->getNumChildNodes();
		}
		return 0;
	}

	std::shared_ptr<PropertyNode> PropertyValueImpl::getChildNode(int32_t childIndex) const
	{
		std::shared_ptr<PropertyNode> propertyNodeLock = m_propertyNode.lock();
		if (propertyNodeLock)
		{
			return propertyNodeLock->getChildNode(childIndex);
		}
		return 0;
	}

	std::shared_ptr<PropertyNode> PropertyValueImpl::getChildNode(const GuGuUtf8Str& childName, bool bRecurse) const
	{
		std::shared_ptr<PropertyNode> propertyNodeLock = m_propertyNode.lock();
		if (propertyNodeLock)
		{
			return propertyNodeLock->findChildPropertyNode(childName, bRecurse);
		}
		return 0;
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
	IMPLEMENT_PROPERTY_ACCESSOR(double)
	IMPLEMENT_PROPERTY_ACCESSOR(math::double3)
	IMPLEMENT_PROPERTY_ACCESSOR(AssetData)
	IMPLEMENT_PROPERTY_ACCESSOR(int8_t)
	IMPLEMENT_PROPERTY_ACCESSOR(int16_t)
	IMPLEMENT_PROPERTY_ACCESSOR(int32_t)
	IMPLEMENT_PROPERTY_ACCESSOR(int64_t)
	IMPLEMENT_PROPERTY_ACCESSOR(uint8_t)
	IMPLEMENT_PROPERTY_ACCESSOR(uint16_t)
	IMPLEMENT_PROPERTY_ACCESSOR(uint32_t)
	IMPLEMENT_PROPERTY_ACCESSOR(uint64_t)
	IMPLEMENT_PROPERTY_ACCESSOR(GameObjectLevelRef)

	PropertyHandleBase::PropertyHandleBase(std::shared_ptr<PropertyNode> propertyNode)
		: m_implementation(std::make_shared<PropertyValueImpl>(propertyNode))
	{

	}

	std::shared_ptr<Widget> PropertyHandleBase::createPropertyNameWidget(const GuGuUtf8Str& nameOverride /*= ""*/)
	{
		if (m_implementation && m_implementation->getPropertyNode())
		{
			std::shared_ptr<PropertyEditor> propertyEditor = std::make_shared<PropertyEditor>(m_implementation->getPropertyNode());

			return WIDGET_NEW(PropertyNameWidget, propertyEditor);
		}

		return NullWidget::getNullWidget();
	}

	PropertyAccess::Result PropertyHandleBase::getNumChildren(uint32_t& outNumChildren) const
	{
		outNumChildren = m_implementation->getNumChildren();
		return PropertyAccess::Success;
	}

	std::shared_ptr<IPropertyHandle> PropertyHandleBase::getChildHandle(uint32_t index) const
	{
		std::shared_ptr<PropertyNode> propertyNode = m_implementation->getChildNode(index);
		return PropertyEditorHelps::getPropertyHandle(propertyNode);
	}

	std::shared_ptr<IPropertyHandle> PropertyHandleBase::getChildHandle(const GuGuUtf8Str& childName, bool bRecurse) const
	{
		std::shared_ptr<PropertyNode> propertyNode = m_implementation->getChildNode(childName, bRecurse);
		return PropertyEditorHelps::getPropertyHandle(propertyNode);
	}

	PropertyAccess::Result PropertyHandleBase::setValueFromFormattedString(const GuGuUtf8Str& inValue)
	{
		return m_implementation->setValueAsString(inValue);
	}

	PropertyAccess::Result PropertyHandleBase::getValueAsFormattedString(GuGuUtf8Str& outValue) const
	{
		return m_implementation->getValueAsDisplayString(outValue);
	}

	const meta::Field* PropertyHandleBase::getField() const
	{
		std::shared_ptr<PropertyNode> propertyNode = m_implementation->getPropertyNode();
		if (propertyNode && propertyNode->getField())
		{
			return propertyNode->getField();
		}
		return nullptr;
	}

	std::shared_ptr<PropertyNode> PropertyHandleBase::getPropertyNode() const
	{
		return m_implementation->getPropertyNode();
	}

	void PropertyHandleBase::getOuterObjects(std::vector<meta::Object*>& outerObjects) const
	{
		if (m_implementation->getPropertyNode())
		{
			//std::vector<meta::Object*> objectsToModify;
			ComplexPropertyNode* complexNode = m_implementation->getPropertyNode()->findComplexParent();
			if (complexNode)
			{
				const int32_t numObjects = complexNode->getInstancesNum();
				for (int32_t index = 0; index < numObjects; ++index)
				{
					outerObjects.push_back(complexNode->getInstanceAsObject(index));
				}
			}
		}
	}

#define IMPLEMENT_PROPERTY_VALUE(ClassName)\
	ClassName::ClassName(std::shared_ptr<PropertyNode> propertyNode) \
	: PropertyHandleBase(propertyNode)\
	{}

	IMPLEMENT_PROPERTY_VALUE(PropertyHandleFloat)
	IMPLEMENT_PROPERTY_VALUE(PropertyHandleObject)
	IMPLEMENT_PROPERTY_VALUE(PropertyHandleGameObjectRef)
	IMPLEMENT_PROPERTY_VALUE(PropertyHandleRotator)

	bool PropertyHandleFloat::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if(field == nullptr)
		{
			return false;
		}
		return field->GetType() == typeof(float);
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


	PropertyHandleVector3::PropertyHandleVector3(std::shared_ptr<PropertyNode> propertyNode)
		: PropertyHandleBase(propertyNode)
	{

	}

	bool PropertyHandleVector3::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if (field == nullptr)
		{
			return false;
		}
		return field->GetType() == typeof(math::double3);
	}

	PropertyAccess::Result PropertyHandleVector3::getValue(math::double3& outValue) const
	{
		return PropertyAccess::Result::Success;
	}

	PropertyAccess::Result PropertyHandleVector3::setValue(const math::double3& inValue)
	{
		return PropertyAccess::Result::Success;
	}

	PropertyHandleDouble::PropertyHandleDouble(std::shared_ptr<PropertyNode> propertyNode)
		: PropertyHandleBase(propertyNode)
	{

	}

	bool PropertyHandleDouble::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if (field == nullptr)
		{
			return false;
		}
		return field->GetType() == typeof(double);
	}
	
	PropertyHandleInt::PropertyHandleInt(std::shared_ptr<PropertyNode> propertyNode)
		: PropertyHandleBase(propertyNode)
	{

	}

	bool PropertyHandleInt::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if (field == nullptr)
		{
			return false;
		}
		const bool bIsInteger = 
			   field->GetType() == typeof(int8_t)
			|| field->GetType() == typeof(int16_t)
			|| field->GetType() == typeof(int32_t)
			|| field->GetType() == typeof(int64_t)
			|| field->GetType() == typeof(uint8_t)
			|| field->GetType() == typeof(uint16_t)
			|| field->GetType() == typeof(uint32_t)
			|| field->GetType() == typeof(uint64_t);
		return bIsInteger;
	}

	PropertyAccess::Result PropertyHandleInt::getValue(int8_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(int16_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(int32_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(int64_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(uint8_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(uint16_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(uint32_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::getValue(uint64_t& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToInt();

		return res;
	}

	PropertyAccess::Result PropertyHandleInt::setValue(const int8_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const int16_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const int32_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const int64_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%lld", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const uint8_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const uint16_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const uint32_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	GuGu::PropertyAccess::Result PropertyHandleInt::setValue(const uint64_t& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%I64d", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	PropertyAccess::Result PropertyHandleDouble::getValue(double& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();
		outValue = fieldValue.ToDouble();

		return res;
	}

	PropertyAccess::Result PropertyHandleDouble::setValue(const double& inValue)
	{
		PropertyAccess::Result res;

		char buffer[64];
		std::sprintf(buffer, "%.3lf", inValue);
		GuGuUtf8Str valueStr(buffer);
		res = m_implementation->importText(valueStr);

		return res;
	}

	bool PropertyHandleObject::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if (field == nullptr)
		{
			return false;
		}
		return field->GetType() == typeof(std::shared_ptr<AssetData>);
	}

	PropertyAccess::Result PropertyHandleObject::getValue(AssetData& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();

		if(fieldValue.IsValid())
			outValue = *fieldValue.GetValue<std::shared_ptr<AssetData>>();

		return res;
	}

	PropertyAccess::Result PropertyHandleObject::setValue(const AssetData& inValue)
	{
		PropertyAccess::Result res;
		GuGuUtf8Str valueStr = AssetManager::getAssetManager().getGuid(std::make_shared<AssetData>(inValue)).getGuid();
		res = m_implementation->importText(valueStr);

		return res;
	}

	bool PropertyHandleRotator::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if (field == nullptr)
		{
			return false;
		}
		return field->GetType() == typeof(math::Rotator);
	}

	PropertyAccess::Result PropertyHandleRotator::getValue(math::double3& outValue) const
	{
		return PropertyAccess::Result::Success;
	}

	PropertyAccess::Result PropertyHandleRotator::setValue(const math::double3& inValue)
	{
		return PropertyAccess::Result::Success;
	}

	bool PropertyHandleGameObjectRef::supports(std::shared_ptr<PropertyNode> propertyNode)
	{
		meta::Field* field = propertyNode->getField();
		if (field == nullptr)
		{
			return false;
		}
		return field->GetType() == typeof(std::shared_ptr<GameObjectLevelRef>);
	}

	PropertyAccess::Result PropertyHandleGameObjectRef::getValue(GameObjectLevelRef& outValue) const
	{
		meta::Variant fieldValue;
		PropertyAccess::Result res = m_implementation->getValueData(fieldValue);
		//outValue = m_implementation->getPropertyValue();

		if (fieldValue.IsValid())
			outValue = *fieldValue.GetValue<std::shared_ptr<GameObjectLevelRef>>();

		return res;
	}

	PropertyAccess::Result PropertyHandleGameObjectRef::setValue(const GameObjectLevelRef& inValue)
	{
		PropertyAccess::Result res;
		GuGuUtf8Str valueStr = inValue.toString();
		res = m_implementation->importText(valueStr);

		return res;
	}

}