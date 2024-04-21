#include <pch.h>

#include "EditableTextBox.h"
#include "BoxPanel.h"
#include "EditableText.h"
#include "StyleSet.h"

namespace GuGu {
    EditableTextBox::EditableTextBox()
    {
        m_style = StyleSet::getStyle()->getStyle<EditableTextBoxStyle>("EditableTextBox");
    }
    EditableTextBox::~EditableTextBox()
    {
    }
    void EditableTextBox::init(const BuilderArguments& arguments)
    {
        Border::init(Border::BuilderArguments()
            .Content(
                WIDGET_NEW(HorizontalBox)
                + HorizontalBox::Slot()
                .setHorizontalAlignment(HorizontalAlignment::Left)
                .StretchWidth(1.0f)
                //.setPadding(Padding(5.0f, 5.0f, 5.0f, 5.0f))
                (
                    WIDGET_ASSIGN_NEW(EditableText, m_editableText)
                    .text(arguments.mText)
                )
            )
            //.Clip(WidgetClipping::ClipToBounds)
            .brush(this, &EditableTextBox::getBorderImage)
            .horizontalAlignment(HorizontalAlignment::Left)
        );
        m_visibilityAttribute = arguments.mVisibility;
    }

    bool EditableTextBox::supportsKeyboardFocus() const
    {
        return m_editableText->supportsKeyboardFocus();
    }

    Reply EditableTextBox::OnFocusReceived(const WidgetGeometry& myGeometry)
    {
        Reply reply = Reply::Handled();

        //转发键盘焦点到我们的编辑文本控件
        reply.setFocus(m_editableText);

        return reply;
    }

    std::shared_ptr<Brush> EditableTextBox::getBorderImage() const
    {
        return m_style->m_backgroundImageNormal;
    }

}