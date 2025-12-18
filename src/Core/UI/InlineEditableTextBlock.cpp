#include <pch.h>

#include "InlineEditableTextBlock.h"
#include <Core/UI/BoxPanel.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/EditableTextBox.h>

#include <Application/Application.h>

namespace GuGu {

	InlineEditableTextBlock::InlineEditableTextBlock()
	{

	}

	InlineEditableTextBlock::~InlineEditableTextBlock()
	{

	}

	void InlineEditableTextBlock::init(const BuilderArguments& arguments)
	{
		m_OnTextCommittedCallback = arguments.monTextCommitted;
		m_text = arguments.mtext; //attribute

		WIDGET_ASSIGN_NEW(HorizontalBox, m_horizontalBox)
		+ HorizontalBox::Slot()
		.setVerticalAlignment(VerticalAlignment::Center)
		.FixedWidth()
		(
			WIDGET_ASSIGN_NEW(TextBlockWidget, m_textBlock)
			.textColor(arguments.mtextColor)
			.text(arguments.mtext)
		);

		WIDGET_ASSIGN_NEW(EditableTextBox, m_textBox)
		.Text(arguments.mtext)
		.onTextCommitted(this, &InlineEditableTextBlock::onTextBoxCommitted);

		m_childWidget = std::make_shared<SingleChildSlot>();//slot
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = m_horizontalBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	bool InlineEditableTextBlock::supportsKeyboardFocus() const
	{
		//如果这个控件的状态选择被其他控件管理，则不能有键盘焦点
		if(m_isSelected)
			return true;
		return false;
	}

	Reply InlineEditableTextBlock::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (!inMouseEvent.isMouseButtonDown(Keys::LeftMouseButton) || inMouseEvent.isControlDown() || inMouseEvent.isShiftDown())
		{
			return Reply::Unhandled();
		}

		if (hasKeyboardFocus())
		{
			enterEditingMode();
			return Reply::Handled();
		}

		return Reply::Unhandled();
	}

	Reply InlineEditableTextBlock::OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return Reply::Handled();
	}

	Reply InlineEditableTextBlock::OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent)
	{
		return Reply::Handled();
	}

	void InlineEditableTextBlock::enterEditingMode()
	{
		if (Application::getApplication()->hasCapture() == false)
		{
			if (m_textBlock->getVisibility() == Visibility::Visible)
			{
				const GuGuUtf8Str currentText = m_textBlock->getText();
				setEditableText(currentText);

				std::shared_ptr<Widget> activeTextBox = getEditableTextWidget();
				m_horizontalBox->addSlot()
				(
					activeTextBox
				);

				m_textBlock->setVisibility(Visibility::Collapsed);
				m_widgetToFocus = Application::getApplication()->getKeyboardFocusedWidget();
				Application::getApplication()->setKeyboardFocus(activeTextBox);

				m_textBlock->setVisibility(Visibility::Collapsed);
			}
		}
	}

	void InlineEditableTextBlock::exitEditingMode()
	{
		m_horizontalBox->removeSlot(getEditableTextWidget());
		m_textBlock->setVisibility(Visibility::Visible);

		std::shared_ptr<Widget> widgetToFocusLocked = m_widgetToFocus.lock();
		if (widgetToFocusLocked)
		{
			Application::getApplication()->setKeyboardFocus(widgetToFocusLocked);
		}
		else
		{
			Application::getApplication()->clearKeyboardFocus();
		}
	}

	void InlineEditableTextBlock::setEditableText(const Attribute<GuGuUtf8Str>& inNewText)
	{
		m_textBox->setText(inNewText);
	}

	std::shared_ptr<Widget> InlineEditableTextBlock::getEditableTextWidget() const
	{
		return m_textBox;
	}

	void InlineEditableTextBlock::onTextBoxCommitted(const GuGuUtf8Str& inText, TextCommit::Type inCommitType)
	{
		if (inCommitType == TextCommit::OnCleared)
		{
			GuGuUtf8Str sourceTextInEditMode = m_text.Get();
			
			cancelEditMode();

			if (m_OnTextCommittedCallback)
			{
				m_OnTextCommittedCallback(sourceTextInEditMode, inCommitType);
			}
		}
		else if(isInEditMode())
		{
			exitEditingMode();

			if (m_OnTextCommittedCallback)
			{
				m_OnTextCommittedCallback(inText, inCommitType);
			}

			if (!m_text.IsBound())
			{
				m_textBlock->setText(m_text);
			}
		}
	}

	void InlineEditableTextBlock::cancelEditMode()
	{
		exitEditingMode();

		setEditableText(m_text);
	}

	bool InlineEditableTextBlock::isInEditMode()
	{
		return m_textBlock->getVisibility() == Visibility::Collapsed;
	}

}