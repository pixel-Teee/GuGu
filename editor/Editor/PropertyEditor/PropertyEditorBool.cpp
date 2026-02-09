#include <pch.h>

#include "PropertyEditorBool.h"

#include <Core/UI/BoxPanel.h>
#include <Core/UI/EditableTextBox.h>
#include <Core/UI/NullWidget.h>
#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	void PropertyEditorBool::init(const BuilderArguments& arguments,
		std::shared_ptr<PropertyEditor> inPropertyEditor)
	{
		m_propertyEditor = inPropertyEditor;
		//todo:check muti line

		m_checkBox = WIDGET_NEW(CheckBox)
			.onCheckStateChanged(this, &PropertyEditorBool::onCheckStateChanged)
			.isChecked(Attribute<CheckBoxState>::CreateSP(this, &PropertyEditorBool::onGetCheckState))
			.checkBoxStyle(EditorStyleSet::getStyleSet()->getStyle<CheckBoxStyle>("EditorNormalCheckBox"))
			.visibility(Visibility::Visible)
			.Content
			(
				NullWidget::getNullWidget()
			);
		
		m_visibilityAttribute = Visibility::SelfHitTestInvisible;
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = m_checkBox;
	}

	bool PropertyEditorBool::supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
		const meta::Field* property = inPropertyEditor->getField();

		if (property->GetType() == typeof(bool))
		{
			return true;
		}

		return false;
	}

	void PropertyEditorBool::getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
	{
		outMinFixedWidth = 125.0f;
		outMaxFixedWidth = 600.0f;
	}

	void PropertyEditorBool::onCheckStateChanged(CheckBoxState inNewState)
	{
		std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();
		if (inNewState == CheckBoxState::Checked)
		{
			propertyHandle->setValue(true);
		}
		else
		{
			propertyHandle->setValue(false);
		}
	}


	CheckBoxState PropertyEditorBool::onGetCheckState() const
	{
		CheckBoxState returnState = CheckBoxState::Unchecked;

		bool value;
		const std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();
		if (propertyHandle->getValue(value) == PropertyAccess::Success)
		{
			if (value == true)
			{
				returnState = CheckBoxState::Checked;
			}
			else
			{
				returnState = CheckBoxState::Unchecked;
			}
		}
		return returnState;
	}

}