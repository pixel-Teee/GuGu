#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/EditableTextBox.h>

#include "PropertyEditor.h"
#include "PropertyHandle.h"

namespace GuGu {
	class EditableTextBox;
	class PropertyEditorText : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyEditorText>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor);

		static bool supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor);

		void getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth);

		void onTextCommitted(const GuGuUtf8Str& newText, TextCommit::Type);
	private:
		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<EditableTextBox> m_singleLineWidget;
	};
}