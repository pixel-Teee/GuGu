#pragma once

#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include <memory>

namespace GuGu {
	class PropertyNode;
	class DetailCategoryImpl;
	class PropertyEditor;
	class IPropertyHandle;
	class IPropertyTypeCustomization;
	class DetailPropertyRow : public IDetailPropertyRow, public std::enable_shared_from_this<DetailPropertyRow>
	{
	public:
		DetailPropertyRow(std::shared_ptr<PropertyNode> inPropertyNode, std::shared_ptr<DetailCategoryImpl> inParentCategory);

		std::shared_ptr<PropertyNode> getPropertyNode() const { return m_propertyNode; }

		DetailWidgetRow getWidgetRow();

		static std::shared_ptr<PropertyEditor> makePropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode, std::shared_ptr<PropertyEditor>& inPropertyEditor);;

		void onItemNodeInitialized(std::shared_ptr<DetailCategoryImpl> inParentCategory);

		std::shared_ptr<IPropertyTypeCustomization> getTypeInterface();
	private:
		void makeNameOrKeyWidget(DetailWidgetRow& row, std::shared_ptr<DetailWidgetRow> inCustomPropertyWidget);

		void makeValueWidget(DetailWidgetRow& row, std::shared_ptr<DetailWidgetRow> inCustomPropertyWidget);

		std::shared_ptr<PropertyNode> m_propertyNode;

		std::weak_ptr<DetailCategoryImpl> m_parentCategory;

		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<DetailWidgetRow> m_customPropertyWidget;//自定义控件，针对color，math::float4等

		std::shared_ptr<IPropertyHandle> m_propertyHandle;
	};
}