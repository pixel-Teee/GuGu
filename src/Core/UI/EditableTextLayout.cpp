#include <pch.h>

#include "EditableTextLayout.h"
#include "GuGuTextLayout.h"
#include "WidgetGeometry.h"
#include "IEditableTextWidget.h"
#include "PlainTextLayoutMarshaller.h"
#include "Events.h"
#include "Widget.h"

#ifdef WIN32
#include <Application/Platform/Windows/WindowsMisc.h>
#else
#ifdef ANDROID
#include <Application/Platform/Android/AndroidMisc.h>
#endif
#endif

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
        m_textSelectionHighlighter = EditableTextTypes::TextSelectionHighlighter::Create();

		m_textLayout->clearLines();
		m_marshaller->setText(m_boundText.Get(), *m_textLayout);

        m_bIsDragSelecting = false;
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
            {
				Reply reply = boolToReply(handleBackspace());

				const GuGuUtf8Str editedText = getEditableText();
				m_boundText = editedText;

				return reply;
            }
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
                if (m_ownerWidget->canTypeCharacter(Character))
                {
					//end edit transaction
					Reply reply = boolToReply(handleTypeChar(Character));

					const GuGuUtf8Str editedText = getEditableText();
					m_boundText = editedText;

					return reply;
                }               
            }
        }

        return Reply::Unhandled();
    }
    Reply EditableTextLayout::handleKeyDown(const KeyEvent& inKeyEvent)
    {
        Reply reply = Reply::Unhandled();

        const Key key = inKeyEvent.getKey();

        if (key == Keys::Left)
        {
            moveCursor(MoveCursor::cardinal(CursorMoveGranularity::Word, math::int2(-1, 0), CursorAction::MoveCursor));
            reply = Reply::Handled();
        }
        else if (key == Keys::Right)
        {
            moveCursor(MoveCursor::cardinal(CursorMoveGranularity::Word, math::int2(+1, 0), CursorAction::MoveCursor));
            reply = Reply::Handled();
        }
        else if (key == Keys::C && inKeyEvent.isControlDown())
        {
            copySelectedTextToClipboard();
            reply = Reply::Handled();
        }
        else if (key == Keys::V && inKeyEvent.isControlDown())
        {
            pasteTextFromClipboard();
            reply = Reply::Handled();
        }

		//if (key == Keys::BackSpace)
		//{
		//    //move left
		//    moveCursor(MoveCursor::cardinal(CursorMoveGranularity::Word, math::int2(-1, 0), CursorAction::SelectText));
		//    reply = boolToReply(handleBackspace());
		//}

        return reply;
    }
    Reply EditableTextLayout::handleMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        moveCursor(MoveCursor::viaScreenPointer(myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition), myGeometry.mAbsoluteScale, CursorAction::MoveCursor));

		//开始拖动选择
		m_bIsDragSelecting = true;

        Reply reply = Reply::Handled();
        reply.captureMouse(m_ownerWidget->getWidget());
        reply.setFocus(m_ownerWidget->getWidget());

        return reply;
    }
    Reply EditableTextLayout::handleMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        if (m_ownerWidget->getWidget()->hasMouseCapture())
        {
            m_bIsDragSelecting = false;
        }

		Reply reply = Reply::Handled();
        reply.releaseMouseCapture();
		
		return reply;
    }
    Reply EditableTextLayout::handleMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        if (m_bIsDragSelecting && m_ownerWidget->getWidget()->hasAnyFocus() && (inMouseEvent.getCursorDelta().x != 0 || inMouseEvent.getCursorDelta().y != 0))
        {
            moveCursor(MoveCursor::viaScreenPointer(myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition), myGeometry.mAbsoluteScale, CursorAction::SelectText));
            return Reply::Handled();
        }

        return Reply::Unhandled();
    }
    void EditableTextLayout::handleFocusLost()
    {
        clearSelection();

        //之后要根据focus event来判断
        TextCommit::Type textAction;
        textAction = TextCommit::OnUserMovedFocus;

        const GuGuUtf8Str editedText = getEditableText();
        m_ownerWidget->onTextCommitted(editedText, textAction);

        updateCursorHighlight();
    }
    bool EditableTextLayout::handleTypeChar(const GuGuUtf8Str& inChar)
    {
        if (anyTextSelected())
        {
            //删除选中的文本
            deleteSelectedText();
        }

        const bool bIsCharAllowed = isCharAllowed(inChar.getUtf16String().at(0));
        if (bIsCharAllowed)
        {
			const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
			const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();
			const TextLayout::LineModel& line = lines[cursorInteractionPosition.getLineIndex()];

			m_textLayout->insertAt(cursorInteractionPosition, inChar, true);

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

        

        if (inArgs.getMoveMethod() == CursorMoveMethod::Cardinal)
        {
            newCursorPosition = translatedLocation(cursorPosition, inArgs.getMoveDirection().x);
        }
        else if (inArgs.getMoveMethod() == CursorMoveMethod::ScreenPosition)
        {
            newCursorPosition = m_textLayout->getTextLocationAt(inArgs.getLocalPosition() * inArgs.getGeometryScale());
        }

        if (inArgs.getAction() == CursorAction::SelectText)
        {
            if (!m_selectionStart.has_value())
            {
                m_selectionStart = cursorPosition;
            }
        }
        else
        {
            //clear selection
            m_selectionStart.reset();
        }

        //if (m_selectionStart.has_value())
        //{
        //    //GuGu_LOGD("%d, %d", m_selectionStart->getOffset(), newCursorPosition.getOffset());
        //}
        
        m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, newCursorPosition);

        updateCursorHighlight();

        return true;
    }
    bool EditableTextLayout::handleBackspace()
    {
        if (anyTextSelected())
        {
            //删除选中的文本
            deleteSelectedText();
        }
        else
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

				m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, finalCursorLocation);

				updateCursorHighlight();
			}
        }
        return true;
    }

    void EditableTextLayout::insertTextAtCursorImpl(const GuGuUtf8Str& inString)
    {
        std::vector<TextRange> lineRanges;
        TextRange::CalculateLineRangesFromString(inString, lineRanges);//参数是要插入的文本

        //插入每一行
        {
            bool bIsFirstLine = true;
            for (const TextRange& lineRange : lineRanges)
            {
                if (!bIsFirstLine)
                {
                    const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
                    if (m_textLayout->splitLineAt(cursorInteractionPosition))//分割光标出的原先的文本
                    {
                        const TextLocation newCursorPosition = TextLocation(cursorInteractionPosition.getLineIndex() + 1, 0);
                        m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, newCursorPosition);
                    }
                }
                bIsFirstLine = false;

                const GuGuUtf8Str newLineText = inString.substr(lineRange.m_beginIndex, lineRange.len());

                const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
                const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();
                const TextLayout::LineModel& line = lines[cursorInteractionPosition.getLineIndex()];

                //插入文本在光标位置
                m_textLayout->insertAt(cursorInteractionPosition, newLineText);

                const TextLocation newCursorPosition = TextLocation(cursorInteractionPosition.getLineIndex(), std::min(cursorInteractionPosition.getOffset() + newLineText.len(), line.text->len()));
                m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, newCursorPosition);
            }

            updateCursorHighlight();
        }
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
        static const int32_t selectionHighlightZOrder = -10;//绘制在文本下面
        static const int32_t cursorZOrder = 11;//绘制在文本上面

        const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
        const TextLocation selectionLocation = m_selectionStart.value_or(cursorInteractionPosition);

        const bool bHasKeyboardFocus = m_ownerWidget->getWidget()->hasAnyFocus();//检查自己是否有焦点
        const bool bHasSelection = selectionLocation != cursorInteractionPosition;

        if (bHasSelection)
        {
            const TextSelection selection(selectionLocation, cursorInteractionPosition);

            const int32_t selectionBeginningLineIndex = selection.getBeginning().getLineIndex();
            const int32_t selectionBeginningLineOffset = selection.getBeginning().getOffset();

            const int32_t selectionEndLineIndex = selection.getEnd().getLineIndex();
            const int32_t selectionEndLineOffset = selection.getEnd().getOffset();
            
            m_textSelectionHighlighter->setHasKeyboardFocus(bHasKeyboardFocus);

            //GuGu_LOGD("%d, %d 更新选择", selectionBeginningLineOffset, selectionEndLineOffset);

            if (selectionBeginningLineIndex == selectionEndLineIndex)
            {
                const TextRange range(selectionBeginningLineOffset, selectionEndLineOffset);
                m_activeLineHighlights.push_back(TextLineHighlight(selectionBeginningLineIndex, range, selectionHighlightZOrder, m_textSelectionHighlighter));
            }
            else
            {
                //todo:多行处理
            }
        }

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
    void EditableTextLayout::copySelectedTextToClipboard()
    {
        if (anyTextSelected())
        {
            const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
            TextLocation selectionLocation = m_selectionStart.value_or(cursorInteractionPosition);
            TextSelection selection(selectionLocation, cursorInteractionPosition);

            //获取抓取的子串
            GuGuUtf8Str selectedText;
            m_textLayout->getSelectionAsText(selectedText, selection);

            //拷贝文本到剪切板
            PlatformMisc::ClipboardCopy(selectedText);
        }
    }
    void EditableTextLayout::pasteTextFromClipboard()
    {
        //删除选择的文本
        deleteSelectedText();

        //从剪切板获取文本
        GuGuUtf8Str pastedText;
        PlatformMisc::ClipboardPaste(pastedText);

        if (pastedText.len() > 0)
        {
            insertTextAtCursorImpl(pastedText);
            m_textLayout->updateIfNeeded();
        }
    }
    bool EditableTextLayout::anyTextSelected() const
    {
        const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
        const TextLocation selectionPosition = m_selectionStart.value_or(cursorInteractionPosition);
        return selectionPosition != cursorInteractionPosition;
    }
    void EditableTextLayout::deleteSelectedText()
    {
        if (anyTextSelected())
        {
            const TextLocation cursorInteractionPosition = m_cursorInfo.getCursorInteractionLocation();
            TextLocation selectionLocation = m_selectionStart.value_or(cursorInteractionPosition);
            TextSelection selection(selectionLocation, cursorInteractionPosition);

            int32_t selectionBeginningLineIndex = selection.getBeginning().getLineIndex();
            int32_t selectionBeginningLineOffset = selection.getBeginning().getOffset();

            int32_t selectionEndLineIndex = selection.getEnd().getLineIndex();
            int32_t selectionEndLineOffset = selection.getEnd().getOffset();

            if (selectionBeginningLineIndex == selectionEndLineIndex)
            {
                m_textLayout->removeAt(TextLocation(selectionBeginningLineIndex, selectionBeginningLineOffset), selectionEndLineOffset - selectionBeginningLineOffset);
            }
            else
            {
                //todo:多行处理
            }

            clearSelection();
            const TextLocation finalCursorLocation = TextLocation(selectionBeginningLineIndex, selectionBeginningLineOffset);
            m_cursorInfo.setCursorLocationAndCalculateAlignment(*m_textLayout, finalCursorLocation);
            updateCursorHighlight();
        }
    }
    void EditableTextLayout::clearSelection()
    {
        m_selectionStart = std::optional<TextLocation>();
    }
    TextLocation EditableTextLayout::translatedLocation(const TextLocation& currentLocation, int8_t direction) const
    {
        const std::vector<TextLayout::LineModel>& lines = m_textLayout->getLineModels();

        const int32_t newOffsetInLine = (direction > 0) ? currentLocation.getOffset() + 1 : currentLocation.getOffset() - 1;//暂时不用分词器

        if (newOffsetInLine == -1) //没有可用的grapheme去移动
        {
            if (direction > 0)
            {
                if (currentLocation.getLineIndex() < lines.size() - 1)
                {
                    return TextLocation(currentLocation.getLineIndex() + 1, 0);
                }
            }
            else if (currentLocation.getLineIndex() > 0)
            {
                const int32_t newLineIndex = currentLocation.getLineIndex() - 1;
                return TextLocation(newLineIndex, lines[newLineIndex].text->len());
            }

            return currentLocation;
        }

        return TextLocation(currentLocation.getLineIndex(), newOffsetInLine);
    }
}