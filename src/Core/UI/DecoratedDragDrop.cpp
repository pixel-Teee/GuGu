#include <pch.h>

#include "DecoratedDragDrop.h"
#include "Border.h"
#include "BoxPanel.h"
#include "ImageWidget.h"
#include "TextBlockWidget.h"

namespace GuGu {
	std::shared_ptr<Brush> DecoratedDragDropOp::getIcon() const
	{
		return m_currentIconBrush;
	}
	GuGuUtf8Str DecoratedDragDropOp::getHoverText() const
	{
		return m_currentHoverText;
	}
	std::shared_ptr<Widget> DecoratedDragDropOp::getDefaultDecorator() const
	{
		//create hover widget
		return WIDGET_NEW(Border)
			.BorderBackgroundColor(math::float4(0.65f, 0.56f, 0.45f, 1.0f)) //beige7
			.padding(Padding(2.0f))
			.Content
			(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.FixedWidth()
				.setPadding(Padding(0.0f, 0.0f, 3.0f, 0.0f))
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					WIDGET_NEW(ImageWidget)
					.brush(this, &DecoratedDragDropOp::getIcon)
				)
				+ HorizontalBox::Slot()
				.FixedWidth()
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					WIDGET_NEW(TextBlockWidget)
					.text(this, &DecoratedDragDropOp::getHoverText)
				)
			);
	}
	void DecoratedDragDropOp::resetToDefaultToolTip()
	{
		m_currentHoverText = m_defaultHoverText;
		m_currentIconBrush = m_defaultIconBrush;
		m_currentIconColorAndOpacity = m_defaultIconColorAndOpacity;
	}
}