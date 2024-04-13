#include <pch.h>

#include "IEditableTextWidget.h"
#include "EditableText.h"
#include "Style.h"
#include "StyleSet.h"
#include "PlainTextLayoutMarshaller.h"

namespace GuGu {
	void EditableText::init(const BuilderArguments& arguments)
	{
		TextBlockStyle textStyle = *(StyleSet::getStyle()->getStyle<TextBlockStyle>("normalText"));

		m_plainTextMarshaller = std::make_shared<PlainTextLayoutMarshaller>();

		//TextShapingMethod是塑形方式
		m_editableTextLayout = std::make_unique<EditableTextLayout>(*this, arguments.mtext, textStyle, TextShapingMethod::KerningOnly, m_plainTextMarshaller);

		m_OnIsTypedCharValid = arguments.monIsTypedCharValid;

		m_OnTextCommittedCallback = arguments.monTextCommitted;

		m_visibilityAttribute = arguments.mVisibility;
	}
	uint32_t EditableText::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		const TextBlockStyle& editableTextStyle = m_editableTextLayout->getTextStyle();

		layer = m_editableTextLayout->onGenerateElement(paintArgs, cullingRect, elementList, allocatedGeometry, layer);
		return layer;
	}
	void EditableText::cacheDesiredSize(float inLayoutScaleMultiplier)
	{
		m_editableTextLayout->cacheDesiredSize(inLayoutScaleMultiplier);
		Widget::cacheDesiredSize(inLayoutScaleMultiplier);//compute desired size
	}
	math::float2 EditableText::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 textLayoutSize = m_editableTextLayout->ComputeFixedSize(inLayoutScaleMultiplier);

		return textLayoutSize;
	}
	void EditableText::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) //tick发生在GenerateElement时，在onGenerateElement前
	{
		m_editableTextLayout->Tick(allocatedGeometry, inCurrentTime, inDeltaTime);//note:这个函数会获取bound text 的文本和text layout 所表示的文本进行比较 ，如果发生变动，调用
		//marshaller 的 set text layout ，填充 text layout ，再调用 text layout 的 update if needed 更新 line view 和高亮
	}
	Reply EditableText::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return m_editableTextLayout->handleMouseButtonDown(myGeometry, inMouseEvent);
	}
	Reply EditableText::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return m_editableTextLayout->handleMouseButtonUp(myGeometry, inMouseEvent);
	}
	Reply EditableText::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return m_editableTextLayout->handleMouseMove(myGeometry, inMouseEvent);
	}
	Reply EditableText::OnKeyChar(const WidgetGeometry& myGeometry, const CharacterEvent& inCharacterEvent)
	{
		Reply reply = Reply::Unhandled();

		reply = m_editableTextLayout->handleKeyChar(inCharacterEvent);
		
		return reply;
	}
	Reply EditableText::OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent)
	{
		Reply reply = Reply::Unhandled();

		reply = m_editableTextLayout->handleKeyDown(inKeyEvent);

		return reply;
	}
	bool EditableText::supportsKeyboardFocus() const
	{
		return true;
	}
	void EditableText::OnFocusLost()
	{
		m_editableTextLayout->handleFocusLost();
	}
	std::shared_ptr<Widget> EditableText::getWidget()
	{
		return shared_from_this();
	}

	bool EditableText::canTypeCharacter(const GuGuUtf8Str inChar) const
	{
		if (m_OnIsTypedCharValid)
		{
			return m_OnIsTypedCharValid(inChar);
		}

		return inChar != u8"\t";
	}

	void EditableText::onTextCommitted(const GuGuUtf8Str& inText, const TextCommit::Type inTextAction)
	{
		if (m_OnTextCommittedCallback)
		{
			m_OnTextCommittedCallback(inText, inTextAction);
		}
	}

}