#include <pch.h>

#include "PropertyEditorText.h"

#include <Core/UI/BoxPanel.h>
#include <Core/UI/EditableTextBox.h>

namespace GuGu {

	void PropertyEditorText::init(const BuilderArguments& arguments, 
	std::shared_ptr<PropertyEditor> inPropertyEditor)
	{
		//todo:check muti line

		std::shared_ptr<HorizontalBox> horizontalBox;

		WIDGET_ASSIGN_NEW(HorizontalBox, horizontalBox)
		+ HorizontalBox::Slot()
		.StretchWidth(1.0f)
		(
			WIDGET_ASSIGN_NEW(EditableTextBox, m_singleLineWidget)
			.Text(inPropertyEditor, &PropertyEditor::getValueAsText)
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = horizontalBox;
	}

	bool PropertyEditorText::supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
		const meta::Field* property = inPropertyEditor->getField();

		if (property->GetType() == typeof(GuGuUtf8Str))
		{
			return true;
		}

		return false;
	}

	void PropertyEditorText::getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
	{
		outMinFixedWidth = 125.0f;
		outMaxFixedWidth = 600.0f;
	}

}