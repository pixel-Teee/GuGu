#pragma once

#include "Border.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class EditableTextBoxStyle;
	class EditableTextBox : public Border
	{
	public:
		EditableTextBox();

		virtual ~EditableTextBox();

		struct BuilderArguments : public Arguments<EditableTextBox>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			//ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)
			
			//ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
			//
			//ARGUMENT_VALUE(VerticalAlignment, verticalAlignment)
			//
			//ARGUMENT_VALUE(HorizontalAlignment, horizontalAlignment)
			//
			//ARGUMENT_VALUE(Padding, padding)
			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, Text)
		};

		void init(const BuilderArguments& arguments);
	private:
		std::shared_ptr<Brush> getBorderImage() const;

		std::shared_ptr<EditableTextBoxStyle> m_style;
	};
}