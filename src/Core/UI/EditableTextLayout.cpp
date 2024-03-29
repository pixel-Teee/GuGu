#include <pch.h>

#include "EditableTextLayout.h"
#include "GuGuTextLayout.h"
#include "WidgetGeometry.h"
#include "IEditableTextWidget.h"
#include "PlainTextLayoutMarshaller.h"
#include "Events.h"
#include "Widget.h"

namespace GuGu {
    Reply boolToReply(const bool bHandled)
    {
        return (bHandled) ? Reply::Handled() : Reply::Unhandled();
    }

    bool isCharAllowed(const uint32_t codePoint) 
    {
        if (codePoint == '\t')
            return true;
        else if (codePoint <= 0x1F)
            return false;
        return true;
    }
    EditableTextLayout::EditableTextLayout(IEditableTextWidget& inOwnerWidget, const Attribute<GuGuUtf8Str>& inInitialText, TextBlockStyle inTextStyle, const std::optional<TextShapingMethod> inTextShapingMethod, std::shared_ptr<ITextLayoutMarshaller> inMarshaller)
        : m_ownerWidget(&inOwnerWidget)
        , m_textStyle(inTextStyle)
    {
        m_marshaller = inMarshaller;    
        m_textLayout = std::make_shared<GuGuTextLayout>(inOwnerWidget.getWidget().get(), m_textStyle);
        m_boundText = inInitialText;
        m_cursorLineHighlighter = EditableTextTypes::CursorLineHighlighter::Create(&m_cursorInfo);

		m_textLayout->clearLines();
		m_marshaller->setText(m_boundText.Get(), *m_textLayout);
    }
    EditableTextLayout::~EditableTextLayout()
    {
    }
    const TextBlockStyle& EditableTextLayout::getTextStyle() const
    {
        return m_textStyle;
    }
    void EditableTextLayout::cacheDesiredSize(float layoutScaleMultiplier)
    {
        m_textLayout->setScale(layoutScaleMultiplier);
        //todo:update if needed

        m_textLayout->updateIfNeeded();
        m_textLayout->flowHighlights(); 
        //refresh();
    }
    math::float2 EditableTextLayout::ComputeFixedSize(float layoutScaleMultiplier) const
    {
        return m_textLayout->getSize();
    }
    int32_t EditableTextLayout::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    { 
        m_cachedSize = allocatedGeometry.getLocalSize();

        layer = m_textLayout->OnPaint(paintArgs, allocatedGeometry, cullingRect, elementList, layer, m_textStyle);

        return layer;
    }
    void EditableTextLayout::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
    {
        //refresh();
    }
    bool EditableTextLayout::refresh()
    {
        const GuGuUtf8Str& textToSet = m_boundText.Get();
        return refreshImpl(textToSet);
    }
    bool EditableTextLayout::refreshImpl(const GuGuUtf8Str& inTextToSet)
    {
		m_textLayout->clearLines();
		m_marshaller->setText(inTextToSet, *m_textLayout); //todo:这里可能存在问题，之后修复
        
        m_textLayout->flowHighlights();
        return false;
    }
    Reply EditableTextLayout::handleKeyChar(const CharacterEvent& inCharacterEvent)
    {
        const GuGuUtf8Str Character = inCharacterEvent.getCharacter();

        const uint32_t codePoint = Character.getUnicode()[0];

        switch (codePoint)
        {
            case 8: //back space
            break;

            case '\n': //new line(ctrl + enter)
            break;

            case 1:  //ctrl + a
            case 3:  //ctrl + c
            case 13: //enter
            case 22: //ctrl + v
            case 24: //ctrl + x
            case 25: //ctrl + y
            case 26: //ctrl + z
            case 27: //esc
            case 127: //ctrl + backspace
                return Reply::Handled();
            default:
            {
                //end edit transaction
                Reply reply = boolToReply(handleTypeChar(Character));

                const GuGuUtf8Str editedText = getEditableText();
                m_boundText = editedText;

                return reply;
            }
        }

        return Reply::Unhandled();
    }
    Reply EditableTextLayout::handleKeyDown(const KeyEvent& inKeyEvent)
    {
        Reply reply = Reply::Unhandled();

        const Key key = inKeyEvent.getKey();

        if (key == Keys::BackSpace)
        {
            //move left
            moveCursor(MoveCursor::cardinal(CursorMoveGranularity::Word, math::int2(-1, 0), CursorAction::SelectText));
            reply = boolToReply(handleBackspace());
        }

        return reply;
    }
    Reply EditableTextLayout::handleMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        moveCursor(MoveCursor::viaScreenPointer(myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition), myGeometry.mAbsoluteScale, CursorAction::MoveCursor));

        Reply reply = Reply::Handled();
        reply.captureMouse(m_ownerWidget->getWidget());
        reply.setFocus(m_ownerWidget->getWidget());

        return reply;
    }
    Reply EditableTextLayout::handleMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
		Reply reply = Reply::Handled();
        reply.releaseMouseCapture();
		
		return reply;
    }
    bool EditableTextLayout::handleTypeChar(const GuGuUtf8Str& inChar)
    {
        const bool bIsCharAllowed = isCharAllowed(inChar.getUtf16String().at(0));
        if (bIsCharAllowed)
        {
			const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
			const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();
			const TextLayout::LineModel& line = lines[cursorInteractionPosition.getLineIndex()];

			m_textLayout->insertAt(cursorInteractionPosition, inChar);

			const TextLocation finalCursorLocation = TextLocation(cursorInteractionPosition.getLineIndex(), std::min(cursorInteractionPosition.getOffset() + 1, line.text->len()));

			m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, finalCursorLocation);

			updateCursorHighlight();

			return true;
        }
        return false;
    }
    GuGuUtf8Str EditableTextLayout::getEditableText() const
    {
        GuGuUtf8Str editedText;
        m_marshaller->getText(editedText, *m_textLayout);
        return editedText;
    }
    bool EditableTextLayout::moveCursor(const MoveCursor& inArgs)
    {
        TextLocation newCursorPosition;
        TextLocation cursorPosition = m_cursorInfo.getCursorInteractionLocation();

        if (inArgs.getMoveMethod() == CursorMoveMethod::ScreenPosition)
        {
            newCursorPosition = m_textLayout->getTextLocationAt(inArgs.getLocalPosition() * inArgs.getGeometryScale());
        }

        m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, newCursorPosition);

        updateCursorHighlight();

        return true;
    }
    bool EditableTextLayout::handleBackspace()
    {
        const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
        TextLocation finalCursorLocation = cursorInteractionPosition;

        const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();

        if (cursorInteractionPosition.getOffset() == 0)
        {
            if (cursorInteractionPosition.getLineIndex() > 0)
            {
                const int32_t previousLineIndex = cursorInteractionPosition.getLineIndex() - 1;
                const int32_t cachePreviousLinesCurrentLength = lines[previousLineIndex].text->len();

                //todo:join line with next line
            }
            else
            {
                //删除在caret左边的grapheme
                const TextSelection deleteSelection = m_textLayout->getGraphemeAt(TextLocation(cursorInteractionPosition, -1));
                const int32_t graphemeSize = deleteSelection.getEnd().getOffset() - deleteSelection.getBeginning().getOffset();
                if (m_textLayout->removeAt(deleteSelection.getBeginning(), graphemeSize))
                {
                    finalCursorLocation = TextLocation(cursorInteractionPosition, -graphemeSize);
                }
            }

            m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, finalCursorLocation);
            
            updateCursorHighlight();
        }

        return true;
    }
    void EditableTextLayout::updateCursorHighlight()
    {
        //todo:封装成一个函数
        const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();
        for (const TextLineHighlight& lineHighlight : m_activeLineHighlights)
        {
            if (lineHighlight.m_lineIdex >= 0 && lineHighlight.m_lineIdex < lines.size())
            {
                m_textLayout->removeLineHighlight(lineHighlight);
            }
        }
        m_activeLineHighlights.clear();
        //todo:remove cursor high light
        static const int32_t cursorZOrder = 11;

        const bool bHasKeyboardFocus = m_ownerWidget->getWidget()->hasAnyFocus();//检查自己是否有焦点

        if (bHasKeyboardFocus)
        {
            const TextLocation cursorPosition = m_cursorInfo.getCursorPosition();

            const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();

            if (cursorPosition.getLineIndex() >= 0 && cursorPosition.getLineIndex() < lines.size())
            {
                const TextSelection cursorSelection = m_textLayout->getGraphemeAt(cursorPosition);
                m_activeLineHighlights.push_back(TextLineHighlight(cursorPosition.getLineIndex(), TextRange(cursorSelection.getBeginning().getOffset(), cursorSelection.getEnd().getOffset()), cursorZOrder, m_cursorLineHighlighter));
            }
        }

        for (const TextLineHighlight& lineHighlight : m_activeLineHighlights)
        {
            m_textLayout->addLineHighlight(lineHighlight);
        }
    }
}