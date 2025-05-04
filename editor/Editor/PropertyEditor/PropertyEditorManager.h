#pragma once

#include "PropertyEditorDelegates.h"

namespace GuGu {
	class GuGuUtf8Str;
	class IPropertyTypeCustomization;
	class PropertyNode;
	namespace meta {
		class Field;
	}

	class PropertyEditorManager
	{
	public:
		PropertyEditorManager();

		~PropertyEditorManager();

		static std::shared_ptr<PropertyEditorManager>& getPropertyEditorManager();

		void registerCustomPropertyTypeLayout(const GuGuUtf8Str& propertyTypeName, OnGetPropertyTypeCustomizationInstance inPropertyTypeLayoutGenerateCallback);

		bool isCustomizedStruct(const meta::Field* field);

		std::shared_ptr<IPropertyTypeCustomization> getPropertyTypeCustomization(std::shared_ptr<PropertyNode> inPropertyNode);
	private:

		std::map<GuGuUtf8Str, OnGetPropertyTypeCustomizationInstance> m_globalPropertyTypeToLayoutMap;
	};
}