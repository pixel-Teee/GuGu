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
                .setHorizontalAlignment(HorizontalAlignment::Center)
                .StretchWidth(1.0f)
                (
                    WIDGET_NEW(EditableText)
                    .text(arguments.mText)
                )
            )
            .brush(this, &EditableTextBox::getBorderImage));
        m_visibilityAttribute = arguments.mVisibility;
    }

    std::shared_ptr<Brush> EditableTextBox::getBorderImage() const
    {
        return m_style->m_backgroundImageNormal;
    }

}