#include <pch.h>

#include "ToolTip.h"
#include "NullWidget.h"
#include "Border.h"
#include "TextBlockWidget.h"

namespace GuGu {
	void ToolTip::init(const BuilderArguments& arguments)
	{
		m_textContent = arguments.mtext;
		m_font = arguments.mfont;
		m_colorAndOpacity = arguments.mcolorAndOpacity;
		m_textMargin = arguments.mtextPadding;
		m_borderImage = arguments.mborderImage;
		m_bIsInteractive = arguments.misInteractive;

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		setContentWidget(arguments.mContent->m_childWidget);
	}
	std::shared_ptr<Widget> ToolTip::asWidget()
	{
		return shared_from_this();
	}

	std::shared_ptr<Widget> ToolTip::getContentWidget()
	{
		return m_toolTipContent;
	}

	void ToolTip::setContentWidget(const std::shared_ptr<Widget>& inContentWidget)
	{
		if (inContentWidget != NullWidget::getNullWidget())
		{
			m_widgetContent = inContentWidget;
		}

		std::shared_ptr<Widget> lockedWidgetContent = m_widgetContent.lock();
		if (lockedWidgetContent)
		{
			m_toolTipContent = lockedWidgetContent;

			//自定义内容
			m_childWidget->setChildWidget(
				WIDGET_NEW(Border)
				.brush(m_borderImage)
				.padding(m_textMargin)
				.Content
				(
					m_toolTipContent
				)
			);		
		}
		else
		{
			m_toolTipContent =
				WIDGET_NEW(TextBlockWidget)
				.text(m_textContent)
				.textInfo(m_font)
				.textColor(math::float4(0.19f, 0.16f, 0.13f, 1.0f)); //beige9

			m_childWidget->setChildWidget(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(math::float4(0.65f, 0.56f, 0.45f, 1.0f)) //beige7
				.padding(Padding(2.0f))
				.Content
				(
					WIDGET_NEW(Border)
					.BorderBackgroundColor(math::float4(0.79f, 0.74f, 0.68f, 1.0f)) //beige4
					//.padding(Padding(11.0f))
					.Content
					(
						m_toolTipContent
					)
				)	
			);
		}
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	bool ToolTip::isEmpty() const
	{
		return !m_widgetContent.lock() && !m_textContent.IsSet();
	}

	void ToolTip::onOpening()
	{
		
	}

	void ToolTip::onClosed()
	{
		
	}

	bool ToolTip::isInteractive()
	{
		return m_bIsInteractive.Get();
	}

}