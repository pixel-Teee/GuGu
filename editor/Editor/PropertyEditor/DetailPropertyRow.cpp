#include <pch.h>

#include "DetailPropertyRow.h"
#include "PropertyNode.h"
#include "PropertyEditor.h"
#include "PropertyEditorHelps.h"//PropertyNameWidget
#include "IPropertyTypeCustomization.h"
#include "DetailCategoryBuilderImpl.h"
#include "PropertyEditorManager.h"
#include "DetailLayoutBuilderImpl.h"
#include <Core/UI/BoxPanel.h>

namespace GuGu {

	DetailPropertyRow::DetailPropertyRow(std::shared_ptr<PropertyNode> inPropertyNode, std::shared_ptr<DetailCategoryImpl> inParentCategory)
		: m_propertyNode(inPropertyNode)
		, m_parentCategory(inParentCategory)
	{
		m_propertyHandle = inParentCategory->getParentLayoutImpl().getPropertyHandle(inPropertyNode);
		if (m_propertyNode)
		{
			std::shared_ptr<PropertyNode> propertyNodeRef = m_propertyNode;
			//make property editor
			if (m_propertyNode->asCategoryNode() == nullptr)
			{
				makePropertyEditor(propertyNodeRef, m_propertyEditor);
			}
		}
	}

	DetailWidgetRow DetailPropertyRow::getWidgetRow()
	{
		DetailWidgetRow row;

		//如果m_customPropertyWidget存在，就用m_customPropertyWidget
		makeNameOrKeyWidget(row, m_customPropertyWidget);
		makeValueWidget(row, m_customPropertyWidget);

		return row;
	}

	std::shared_ptr<PropertyEditor> DetailPropertyRow::makePropertyEditor(const std::shared_ptr<PropertyNode>& inPropertyNode, std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		if (!inPropertyEditor)
		{
			inPropertyEditor = std::make_shared<PropertyEditor>(inPropertyNode);
		}
		return inPropertyEditor;
	}

	void DetailPropertyRow::onItemNodeInitialized(std::shared_ptr<DetailCategoryImpl> inParentCategory)
	{
		std::shared_ptr<IPropertyTypeCustomization>& customTypeInterface = getTypeInterface();
		if (!m_customPropertyWidget && customTypeInterface)
		{
			m_customPropertyWidget = std::make_shared<DetailWidgetRow>();

			customTypeInterface->customizeHeader(m_propertyHandle, *m_customPropertyWidget);
		}
	}

	std::shared_ptr<IPropertyTypeCustomization> DetailPropertyRow::getTypeInterface()
	{
		if (m_propertyNode && m_parentCategory.lock())
		{
			return PropertyEditorManager::getPropertyEditorManager()->getPropertyTypeCustomization(m_propertyNode);
		}
		return nullptr;
	}

	void DetailPropertyRow::makeNameOrKeyWidget(DetailWidgetRow& row, std::shared_ptr<DetailWidgetRow> inCustomPropertyWidget)
	{
		VerticalAlignment verticalAlignment = VerticalAlignment::Center;
		HorizontalAlignment horizontalAlignment = HorizontalAlignment::Stretch;

		std::shared_ptr<HorizontalBox> nameHorizontalBox = WIDGET_NEW(HorizontalBox)
			.Clip(WidgetClipping::OnDemand);

		std::shared_ptr<Widget> nameWidget = NullWidget::getNullWidget();
		if (inCustomPropertyWidget)
		{
			nameWidget = inCustomPropertyWidget->m_nameWidget.m_widget;
		}
		else if (m_propertyEditor)
		{
			nameWidget = WIDGET_NEW(PropertyNameWidget, m_propertyEditor);
		}
		nameHorizontalBox->addSlot()
		.FixedWidth()
		(
			nameWidget
		);

		row.nameContent()
		.setHorizontalAlignment(horizontalAlignment)
		.setVerticalAlignment(verticalAlignment)
		(
			nameHorizontalBox
		);
	}

	void DetailPropertyRow::makeValueWidget(DetailWidgetRow& row, std::shared_ptr<DetailWidgetRow> inCustomPropertyWidget)
	{
		VerticalAlignment verticalAlignment = VerticalAlignment::Center;
		HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left;

		std::optional<float> minWidth;
		std::optional<float> maxWidth;

		std::shared_ptr<HorizontalBox> valueWidget = WIDGET_NEW(HorizontalBox);

		if (inCustomPropertyWidget)
		{
			valueWidget->addSlot()
			.FixedWidth()
			(
				inCustomPropertyWidget->m_nameWidget.m_widget
			);
		}
		else if (m_propertyEditor)
		{
			std::shared_ptr<PropertyValueWidget> propertyValue;
			valueWidget->addSlot()
			.FixedWidth()
			(
				WIDGET_ASSIGN_NEW(PropertyValueWidget, propertyValue, m_propertyEditor)
			);

			minWidth = propertyValue->getMinFixedWidth();
			maxWidth = propertyValue->getMaxFixedWidth();
		}


		row.valueContent()
		.setHorizontalAlignment(horizontalAlignment)
		.setVerticalAlignment(verticalAlignment)
		.minFixedWidth(minWidth)
		.maxFixedWidth(maxWidth)
		(
			valueWidget	
		);
	}

}