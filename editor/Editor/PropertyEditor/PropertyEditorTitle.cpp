#include <pch.h>

#include "PropertyEditorTitle.h"
#include "PropertyEditor.h"
#include <Core/UI/TextBlockWidget.h>
#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	void PropertyEditorTitle::init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor)
	{
		m_propertyEditor = inPropertyEditor;

		std::shared_ptr<TextBlockWidget> nameTextBlock;
		nameTextBlock = WIDGET_NEW(TextBlockWidget)
						.text(inPropertyEditor->getDisplayName())
						.textColor(EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9"));

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = nameTextBlock;
	}

}