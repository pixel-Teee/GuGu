#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/ComboBox.h>

#include "PropertyEditor.h"
#include "PropertyHandle.h"

namespace GuGu {
	class PropertyEditorCombo : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyEditorCombo>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor);

		static bool supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor);

		void getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth);

		GuGuUtf8Str getDisplayValueAsString() const;

	private:
		void generateComboBoxStrings(std::vector<std::shared_ptr<GuGuUtf8Str>>& outComboBoxStrings);
		void onComboSelectionChanged(std::shared_ptr<GuGuUtf8Str> newValue, SelectInfo::Type selectInfo);

		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<ComboBox<std::shared_ptr<GuGuUtf8Str>>> m_comoBox;

		void sendToObjects(const GuGuUtf8Str& newValue);

		std::vector<std::shared_ptr<GuGuUtf8Str>> comboItems;
	};
}