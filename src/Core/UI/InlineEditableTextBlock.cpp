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

		WIDGET_ASSIGN_NEW(HorizontalBox, m_horizontalBox)
		+ HorizontalBox::Slot()
		.setVerticalAlignment(VerticalAlignment::Center)
		.FixedWidth()
		(
			WIDGET_ASSIGN_NEW(TextBlockWidget, m_textBlock)
			.text(arguments.mtext)
		);

		m_childWidget->m_childWidget = m_horizontalBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	bool InlineEditableTextBlock::supportsKeyboardFocus() const
	{
		return true;
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

	Reply InlineEditableTextBlock::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return Reply::Handled();
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

}