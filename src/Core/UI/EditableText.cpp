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

		m_editableTextLayout = std::make_unique<EditableTextLayout>(*this, arguments.mtext, textStyle, TextShapingMethod::KerningOnly, m_plainTextMarshaller);
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

        //todo:
		return textLayoutSize;
	}
	void EditableText::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
	{
		m_editableTextLayout->Tick(allocatedGeometry, inCurrentTime, inDeltaTime);
	}
	Reply EditableText::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return m_editableTextLayout->handleMouseButtonDown(myGeometry, inMouseEvent);
	}
	Reply EditableText::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return m_editableTextLayout->handleMouseButtonUp(myGeometry, inMouseEvent);
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
	void EditableText::OnFocusLost()
	{
		m_editableTextLayout->handleFocusLost();
	}
	std::shared_ptr<Widget> EditableText::getWidget()
	{
		return shared_from_this();
	}

}