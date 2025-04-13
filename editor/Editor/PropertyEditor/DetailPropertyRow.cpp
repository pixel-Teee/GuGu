#include <pch.h>

#include "DetailPropertyRow.h"
#include "PropertyNode.h"
#include "PropertyEditor.h"
#include "PropertyEditorHelps.h"//PropertyNameWidget
#include <Core/UI/BoxPanel.h>

namespace GuGu {

	DetailPropertyRow::DetailPropertyRow(std::shared_ptr<PropertyNode> inPropertyNode, std::shared_ptr<DetailCategoryImpl> inParentCategory)
		: m_propertyNode(inPropertyNode)
		, m_parentCategory(inParentCategory)
	{
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

		makeNameOrKeyWidget(row);
		makeValueWidget(row);

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

	void DetailPropertyRow::makeNameOrKeyWidget(DetailWidgetRow& row)
	{
		VerticalAlignment verticalAlignment = VerticalAlignment::Center;
		HorizontalAlignment horizontalAlignment = HorizontalAlignment::Stretch;

		std::shared_ptr<HorizontalBox> nameHorizontalBox = WIDGET_NEW(HorizontalBox)
			.Clip(WidgetClipping::OnDemand);

		std::shared_ptr<Widget> nameWidget = NullWidget::getNullWidget();
		if (m_propertyEditor)
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

	void DetailPropertyRow::makeValueWidget(DetailWidgetRow& row)
	{
		VerticalAlignment verticalAlignment = VerticalAlignment::Center;
		HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left;

		std::optional<float> minWidth;
		std::optional<float> maxWidth;

		std::shared_ptr<HorizontalBox> valueWidget =
		WIDGET_NEW(HorizontalBox);
		if (m_propertyEditor)
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