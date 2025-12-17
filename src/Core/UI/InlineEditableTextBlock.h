#pragma once


#include <Core/UI/Widget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/WidgetDelegates.h>
#include <Core/UI/CompoundWidget.h>

#include <Core/UI/TextInfo.h>

namespace GuGu {
	class TextBlockWidget;
	class EditableTextBox;
	class HorizontalBox;
	//inline editable text(use for rename)
	class InlineEditableTextBlock : public CompoundWidget
	{
	public:
		InlineEditableTextBlock();

		virtual ~InlineEditableTextBlock();

		struct BuilderArguments : public Arguments<InlineEditableTextBlock>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			//显示的文本
			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)

			ARGUMENT_ATTRIBUTE(math::float4, textColor)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<TextInfo>, textInfo)

			//文本提交的回调
			UI_EVENT(OnTextCommitted, onTextCommitted)

			UI_EVENT(IsSelected, isSelected)
		};
		void init(const BuilderArguments& arguments);

		virtual bool supportsKeyboardFocus() const override;

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent) override;

		void enterEditingMode();

		void exitEditingMode();

	private:
		void setEditableText(const Attribute<GuGuUtf8Str>& inNewText);

		std::shared_ptr<Widget> getEditableTextWidget() const;
	protected:
		//label
		std::shared_ptr<TextBlockWidget> m_textBlock;

		//editing
		std::shared_ptr<EditableTextBox> m_textBox;

		OnTextCommitted m_OnTextCommittedCallback;

		std::shared_ptr<HorizontalBox> m_horizontalBox;

		std::weak_ptr<Widget> m_widgetToFocus;

		IsSelected m_isSelected;
	};
}