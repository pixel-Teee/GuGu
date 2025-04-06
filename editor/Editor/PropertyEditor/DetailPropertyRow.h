#pragma once

#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include <memory>

namespace GuGu {
	class PropertyNode;
	class DetailCategoryImpl;
	class PropertyEditor;
	class DetailPropertyRow : public IDetailPropertyRow, public std::enable_shared_from_this<DetailPropertyRow>
	{
	public:
		DetailPropertyRow(std::shared_ptr<PropertyNode> inPropertyNode, std::shared_ptr<DetailCategoryImpl> inParentCategory);

		std::shared_ptr<PropertyNode> getPropertyNode() const { return m_propertyNode; }

		DetailWidgetRow getWidgetRow();

		static std::shared_ptr<PropertyEditor> makePropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode, std::shared_ptr<PropertyEditor>& inPropertyEditor);;
	private:
		void makeNameOrKeyWidget(DetailWidgetRow& row);

		void makeValueWidget(DetailWidgetRow& row);

		std::shared_ptr<PropertyNode> m_propertyNode;

		std::weak_ptr<DetailCategoryImpl> m_parentCategory;

		std::shared_ptr<PropertyEditor> m_propertyEditor;
	};
}