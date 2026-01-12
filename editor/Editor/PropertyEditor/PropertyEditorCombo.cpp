#include <pch.h>

#include "PropertyEditorCombo.h"

#include <Core/UI/BoxPanel.h>
#include <Core/UI/ListView.h>
#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	void PropertyEditorCombo::init(const BuilderArguments& arguments,
		std::shared_ptr<PropertyEditor> inPropertyEditor)
	{
		m_propertyEditor = inPropertyEditor;
		
		generateComboBoxStrings(comboItems);
		std::shared_ptr<GuGuUtf8Str> initiallySelectedItem;
		GuGuUtf8Str initialStr = getDisplayValueAsString();
		for (int32_t i = 0; i < comboItems.size(); ++i)
		{
			if (*comboItems[i] == initialStr)
			{
				initiallySelectedItem = comboItems[i];
				break;
			}
		}

		WIDGET_ASSIGN_NEW(ComboBox<std::shared_ptr<GuGuUtf8Str>>, m_comoBox)
		.visibility(Visibility::Visible)
		.optionSource(&comboItems)
		.onGenerateWidgetLambda([&](std::shared_ptr<GuGuUtf8Str> inOption)->std::shared_ptr<Widget> {
			return WIDGET_NEW(TextBlockWidget)
				.text(*inOption)
				.textColor(Attribute<math::float4>::Create([=]() {
				return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
			}));
		})
		.onSelectionChanged(this, &PropertyEditorCombo::onComboSelectionChanged)
		//.maxListHeight(60.0f)
		.Content
		(
			WIDGET_NEW(Border)
			.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
				return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel4");
			}))
			.Content
			(
				WIDGET_NEW(TextBlockWidget)
				.text(Attribute<GuGuUtf8Str>::Create(std::bind(&PropertyEditorCombo::getDisplayValueAsString, this)))
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
			)
		)
		.initiallySelectedItem(initiallySelectedItem);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = m_comoBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	bool PropertyEditorCombo::supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
		const meta::Field* property = inPropertyEditor->getField();

		if (property->GetType().IsEnum()) //check enum
		{
			return true;
		}

		return false;
	}

	void PropertyEditorCombo::getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
	{
		outMinFixedWidth = 125.0f;
		outMaxFixedWidth = 400.0f;
	}

	GuGuUtf8Str PropertyEditorCombo::getDisplayValueAsString() const
	{
		GuGuUtf8Str valueString;
		if (m_propertyEditor)
		{
			m_propertyEditor->getPropertyHandle()->getValueAsFormattedString(valueString);
		}
		return valueString;
	}

	void PropertyEditorCombo::generateComboBoxStrings(std::vector<std::shared_ptr<GuGuUtf8Str>>& outComboBoxStrings)
	{
		if (m_propertyEditor)
		{
			meta::Field* field = m_propertyEditor->getField();
			if (field)
			{
				meta::Type type = field->GetType();
				if (type.IsEnum())
				{
					std::vector<GuGuUtf8Str> strs = type.GetEnum().GetKeys();
					for (int32_t i = 0; i < strs.size(); ++i)
					{
						outComboBoxStrings.push_back(std::make_shared<GuGuUtf8Str>(strs[i]));
					}
				}
			}
		}
	}

	void PropertyEditorCombo::onComboSelectionChanged(std::shared_ptr<GuGuUtf8Str> newValue, SelectInfo::Type selectInfo)
	{
		if (newValue)
		{
			//send to objects
			sendToObjects(*newValue);
		}
	}

	void PropertyEditorCombo::sendToObjects(const GuGuUtf8Str& newValue)
	{
		if (m_propertyEditor)
		{
			m_propertyEditor->getPropertyHandle()->setValueFromFormattedString(newValue);
		}
	}

}