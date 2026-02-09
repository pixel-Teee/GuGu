#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/CheckBox.h>

#include "PropertyEditor.h"
#include "PropertyHandle.h"

namespace GuGu {
	class EditableTextBox;
	class PropertyEditorBool : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyEditorBool>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor);

		static bool supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor);

		void getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth);

		void onCheckStateChanged(CheckBoxState inNewState);

		CheckBoxState onGetCheckState() const;
	private:
		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<CheckBox> m_checkBox;
	};
}