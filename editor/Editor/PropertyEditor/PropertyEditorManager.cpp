#include <pch.h>

#include "PropertyEditorManager.h"
#include "IPropertyTypeCustomization.h"
#include "Vector4StructCustomization.h"
#include "VectorStructCustomization.h"
#include "Vector2StructCustomization.h"
#include "ColorStructCustomization.h"
#include "RotatorStructCustomization.h"
#include "AnchorDataCustomization.h"
#include "PropertyNode.h"
#include "ObjectDetails.h"
#include <Core/Reflection/Field.h>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {

	PropertyEditorManager::PropertyEditorManager()
	{
		registerCustomPropertyTypeLayout("GuGu::math::float2", Vector2StructCustomization::create);
		registerCustomPropertyTypeLayout("GuGu::math::double2", Vector2StructCustomization::create);
		registerCustomPropertyTypeLayout("GuGu::math::float4", Vector4StructCustomization::create); //四分量
		registerCustomPropertyTypeLayout("GuGu::math::double4", Vector4StructCustomization::create); //四分量
		registerCustomPropertyTypeLayout("GuGu::math::float3", VectorStructCustomization::create); //三分量
		registerCustomPropertyTypeLayout("GuGu::math::double3", RotatorStructCustomization::create); //三分量
		registerCustomPropertyTypeLayout("GuGu::math::Rotator", RotatorStructCustomization::create); //三分量
		registerCustomPropertyTypeLayout("GuGu::Color", ColorStructCustomization::create);
		//registerCustomPropertyTypeLayout("GuGu::UIPadding", Vector4StructCustomization::create);

		//canvas widget
		registerCustomPropertyTypeLayout("GuGu::UIAnchorData", AnchorDataCustomization::create);
	}

	PropertyEditorManager::~PropertyEditorManager()
	{

	}

	void PropertyEditorManager::registerCustomPropertyTypeLayout(const GuGuUtf8Str& propertyTypeName, OnGetPropertyTypeCustomizationInstance inPropertyTypeLayoutGenerateCallback)
	{
		m_globalPropertyTypeToLayoutMap.insert({ propertyTypeName, inPropertyTypeLayoutGenerateCallback });
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
		if (inPropertyNode->getField() != nullptr)
		{
			const GuGuUtf8Str& typeName = inPropertyNode->getField()->GetType().GetName();//check?
			if (m_globalPropertyTypeToLayoutMap.find(typeName) != m_globalPropertyTypeToLayoutMap.end())
				return m_globalPropertyTypeToLayoutMap[typeName]();//execute func
		}
		return nullptr;
	}

	//void PropertyEditorManager::addDetailsView(std::shared_ptr<ObjectDetails> inDetailsView)
	//{
	//	m_allDetailsView.push_back(inDetailsView);
	//}

	//void PropertyEditorManager::refreshDetailsView(const std::vector<GameObject*>& inObjects, bool bForceRefresh)
	//{
	//	//todo:以后要修复
	//	for (int32_t i = 0; i < m_allDetailsView.size(); ++i)
	//	{
	//		m_allDetailsView[i].lock()->setObjects(inObjects, bForceRefresh);
	//	}
	//}

	std::shared_ptr<PropertyEditorManager>& PropertyEditorManager::getPropertyEditorManager()
	{
		static std::shared_ptr<PropertyEditorManager> g_propertyEditorManager = std::make_shared<PropertyEditorManager>();
		return g_propertyEditorManager;
	}

}