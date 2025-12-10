#pragma once

#include "Border.h"
#include "WidgetDelegates.h" //OnTextCommitted
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class EditableText;
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
			UI_EVENT(OnTextCommitted, onTextCommitted)//文本提交的回调

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, Text)
		};

		void init(const BuilderArguments& arguments);

		virtual bool supportsKeyboardFocus() const override;

		virtual Reply OnFocusReceived(const WidgetGeometry& myGeometry);

		void onEditableTextCommitted(const GuGuUtf8Str& inText, TextCommit::Type inCommitType);

		void setText(const Attribute<GuGuUtf8Str>& inNewText);
	private:
		Padding determinePadding() const { return Padding(8.0f, 3.0f, 8.0f, 4.0f); }//todo:fix this, 使用 style 的 padding

		std::shared_ptr<Brush> getBorderImage() const;

		std::shared_ptr<EditableTextBoxStyle> m_style;

		std::shared_ptr<EditableText> m_editableText;

		OnTextCommitted m_onTextCommitted;
	};
}