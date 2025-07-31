#pragma once

#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include "DetailTreeNode.h"//DetailNodeList
#include <memory>

namespace GuGu {
	class PropertyNode;
	class DetailCategoryImpl;
	class PropertyEditor;
	class IPropertyHandle;
	class IPropertyTypeCustomization;
	class CustomChildrenBuilder;
	class DetailPropertyRow : public IDetailPropertyRow, public std::enable_shared_from_this<DetailPropertyRow>
	{
	public:
		DetailPropertyRow(std::shared_ptr<PropertyNode> inPropertyNode, std::shared_ptr<DetailCategoryImpl> inParentCategory);

		std::shared_ptr<PropertyNode> getPropertyNode() const { return m_propertyNode; }

		DetailWidgetRow getWidgetRow();

		static std::shared_ptr<PropertyEditor> makePropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode, std::shared_ptr<PropertyEditor>& inPropertyEditor);;

		void onItemNodeInitialized(std::shared_ptr<DetailCategoryImpl> inParentCategory);

		std::shared_ptr<IPropertyTypeCustomization> getTypeInterface();

		virtual DetailWidgetRow& customWidget(bool bShowChildren = false) override;

		void onGenerateChildren(DetailNodeList& outChildren);

		void generateChildrenForPropertyNode(std::shared_ptr<PropertyNode>& rootPropertyNode, DetailNodeList& outChildren);

		bool showOnlyChildren() const;

		virtual void getDefaultWidgets(std::shared_ptr<Widget>& outNameWidget, std::shared_ptr<Widget>& outValueWidget,
			DetailWidgetRow& row, bool bAddWidgetDecoration = false) override;

		Visibility getPropertyVisibility() const;
	private:
		void makeNameOrKeyWidget(DetailWidgetRow& row, std::shared_ptr<DetailWidgetRow> inCustomPropertyWidget);

		void makeValueWidget(DetailWidgetRow& row, std::shared_ptr<DetailWidgetRow> inCustomPropertyWidget);

		std::shared_ptr<PropertyNode> m_propertyNode;

		std::weak_ptr<DetailCategoryImpl> m_parentCategory;

		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<DetailWidgetRow> m_customPropertyWidget;//自定义控件，针对color，math::float4等

		std::shared_ptr<IPropertyHandle> m_propertyHandle;

		std::shared_ptr<IPropertyTypeCustomization> m_cachedCustomTypeInterface;

		//专门用来生成儿子的CustomizationChildren，会生成一堆DetailItemNode挂在当前DetailItemNode的下面
		std::shared_ptr<CustomChildrenBuilder> m_propertyTypeLayoutBuilder;

		bool m_bCachedCustomTypeInterface;

		bool m_bShowCustomPropertyChildren;
	};
}