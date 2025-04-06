#include <pch.h>

#include "PropertyEditorHelps.h"
#include "PropertyEditorTitle.h"
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Border.h>

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

}