#pragma once

#include "PropertyEditorDelegates.h"

namespace GuGu {
	namespace PropertyAccess
	{
		enum Result
		{
			MultipleValue,
			Fail,
			Success
		};
	}
	class GuGuUtf8Str;
	class IPropertyTypeCustomization;
	class PropertyNode;
	namespace meta {
		class Field;
	}
	//class ObjectDetails;
	class PropertyEditorManager
	{
	public:
		PropertyEditorManager();

		~PropertyEditorManager();

		static std::shared_ptr<PropertyEditorManager>& getPropertyEditorManager();

		void registerCustomPropertyTypeLayout(const GuGuUtf8Str& propertyTypeName, OnGetPropertyTypeCustomizationInstance inPropertyTypeLayoutGenerateCallback);

		bool isCustomizedStruct(const meta::Field* field);

		std::shared_ptr<IPropertyTypeCustomization> getPropertyTypeCustomization(std::shared_ptr<PropertyNode> inPropertyNode);

		//void addDetailsView(std::shared_ptr<ObjectDetails> inDetailsView);
		//
		//void refreshDetailsView(const std::vector<GameObject*>& inObjects, bool bForceRefresh);
	private:

		std::map<GuGuUtf8Str, OnGetPropertyTypeCustomizationInstance> m_globalPropertyTypeToLayoutMap;
		
		//std::vector<std::weak_ptr<ObjectDetails>> m_allDetailsView;
	};
}