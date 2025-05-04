#include <pch.h>

#include "PropertyEditorHelps.h"
#include "PropertyEditorTitle.h"
#include "PropertyNode.h"
#include "PropertyEditor.h"
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Border.h>

#include "PropertyEditorNumeric.h"

namespace GuGu {

	void PropertyNameWidget::init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor)
	{
		m_propertyEditor = inPropertyEditor;
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = 
		WIDGET_NEW(HorizontalBox)
		+ HorizontalBox::Slot()
		.setPadding(Padding(0, 1, 0, 1))
		.FixedWidth()
		(
			WIDGET_NEW(Border)
			.brush(CoreStyle::getStyleSet()->getBrush("NoBorder"))
			.Content
			(
				WIDGET_NEW(PropertyEditorTitle, m_propertyEditor)
			)
		);
	}

	void PropertyValueWidget::init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor)
	{
		m_minFixedWidth = 0.0f;
		m_maxFixedWidth = 0.0f;

		//添加enbaled

		m_valueEditorWidget = constructPropertyEditorWidget(inPropertyEditor);

		m_childWidget = std::make_shared<SingleChildSlot>();
		
		if (m_valueEditorWidget != nullptr)
		{
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget = m_valueEditorWidget;
			m_childWidget->setVerticalAlignment(VerticalAlignment::Center);
		}
		else
		{
			m_childWidget->m_childWidget = NullWidget::getNullWidget();
		}
	}

	std::shared_ptr<Widget> PropertyValueWidget::constructPropertyEditorWidget(std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		const std::shared_ptr<PropertyEditor> propertyEditor = inPropertyEditor;

		const std::shared_ptr<PropertyNode> propertyNode = propertyEditor->getPropertyNode();
		meta::Field* field = propertyNode->getField();

		std::shared_ptr<Widget> propertyWidget;
		if (field)
		{
			if (PropertyEditorNumeric<float>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<float>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<float>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<double>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<double>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<double>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
		}

		if (!propertyWidget)
		{
			//std::shared_ptr<PropertyEditorWidget> basePropertyEditorWidget = 
			//	WIDGET_ASSIGN_NEW(propertyWidget, PropertyEditorWidget, propertyEditor)
			//
			//basePropertyEditorWidget->getFixedWidth()
		}

		return propertyWidget;
	}

}