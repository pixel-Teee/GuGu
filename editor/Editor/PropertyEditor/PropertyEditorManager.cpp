#include <pch.h>

#include "PropertyEditorManager.h"
#include "IPropertyTypeCustomization.h"
#include "Vector4StructCustomization.h"
#include "VectorStructCustomization.h"
#include "PropertyNode.h"
#include <Core/Reflection/Field.h>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {

	PropertyEditorManager::PropertyEditorManager()
	{
		registerCustomPropertyTypeLayout("math::float4", std::make_shared<Vector4StructCustomization>());
		registerCustomPropertyTypeLayout("math::double4", std::make_shared<Vector4StructCustomization>());
		registerCustomPropertyTypeLayout("math::float3", std::make_shared<VectorStructCustomization>());
		registerCustomPropertyTypeLayout("math::double3", std::make_shared<VectorStructCustomization>());
	}

	PropertyEditorManager::~PropertyEditorManager()
	{

	}

	void PropertyEditorManager::registerCustomPropertyTypeLayout(const GuGuUtf8Str& propertyTypeName, std::shared_ptr<IPropertyTypeCustomization> inPropertyTypeLayout)
	{
		m_globalPropertyTypeToLayoutMap.insert({ propertyTypeName, inPropertyTypeLayout });
	}

	bool PropertyEditorManager::isCustomizedStruct(const meta::Field* field)
	{
		const GuGuUtf8Str& propertyTypeName = field->GetType().GetName();
		if (m_globalPropertyTypeToLayoutMap.find(propertyTypeName) != m_globalPropertyTypeToLayoutMap.end())
			return true;
		return false;
	}

	std::shared_ptr<IPropertyTypeCustomization> PropertyEditorManager::getPropertyTypeCustomization(std::shared_ptr<PropertyNode> inPropertyNode)
	{
		const GuGuUtf8Str& typeName = inPropertyNode->getField()->GetType().GetName();//check?
		if (m_globalPropertyTypeToLayoutMap.find(typeName) != m_globalPropertyTypeToLayoutMap.end())
			return m_globalPropertyTypeToLayoutMap[typeName];
		return nullptr;
	}

	std::shared_ptr<PropertyEditorManager>& PropertyEditorManager::getPropertyEditorManager()
	{
		static std::shared_ptr<PropertyEditorManager> g_propertyEditorManager = std::make_shared<PropertyEditorManager>();
		return g_propertyEditorManager;
	}

}