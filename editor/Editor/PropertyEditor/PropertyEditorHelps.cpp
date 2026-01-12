#include <pch.h>

#include "PropertyEditorHelps.h"
#include "PropertyEditorTitle.h"
#include "PropertyNode.h"
#include "PropertyEditor.h"
#include "PropertyHandleImp.h"//property handle float/double
#include "PropertyEditorAsset.h"
#include "PropertyEditorText.h"
#include "PropertyEditorCombo.h"
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
		.setVerticalAlignment(VerticalAlignment::Center)
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
			else if (PropertyEditorNumeric<int8_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<int8_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<int8_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<int16_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<int16_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<int16_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<int32_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<int32_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<int32_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<uint8_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<uint8_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<uint8_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<uint16_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<uint16_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<uint16_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<uint32_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<uint32_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<uint32_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorNumeric<uint64_t>::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorNumeric<uint64_t>> numericWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorNumeric<uint64_t>, numericWidget, inPropertyEditor);
				numericWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = numericWidget;
			}
			else if (PropertyEditorAsset::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorAsset> assetWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorAsset, assetWidget, inPropertyEditor);
				assetWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = assetWidget;
			}
			else if (PropertyEditorText::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorText> textWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorText, textWidget, inPropertyEditor);
				textWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = textWidget;
			}
			else if (PropertyEditorCombo::supports(inPropertyEditor))
			{
				std::shared_ptr<PropertyEditorCombo> comboWidget;
				WIDGET_ASSIGN_NEW(PropertyEditorCombo, comboWidget, inPropertyEditor);
				comboWidget->getFixedWidth(m_minFixedWidth, m_maxFixedWidth);

				propertyWidget = comboWidget;
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

	namespace PropertyEditorHelps
	{
		std::shared_ptr<IPropertyHandle> getPropertyHandle(std::shared_ptr<PropertyNode> inPropertyNode)
		{
			std::shared_ptr<IPropertyHandle> propertyHandle;

			if (PropertyHandleFloat::supports(inPropertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleFloat>(inPropertyNode);
			}
			else if (PropertyHandleDouble::supports(inPropertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleDouble>(inPropertyNode);
			}
			else if (PropertyHandleVector3::supports(inPropertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleVector3>(inPropertyNode);
			}
			else if (PropertyHandleObject::supports(inPropertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleObject>(inPropertyNode);
			}
			else if (PropertyHandleRotator::supports(inPropertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleRotator>(inPropertyNode);
			}
			else if (PropertyHandleInt::supports(inPropertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleInt>(inPropertyNode);
			}
			else
			{
				propertyHandle = std::make_shared<PropertyHandleBase>(inPropertyNode);
			}
			
			return propertyHandle;
		}
	}
}