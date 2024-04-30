#include <pch.h>

#include "ComboButton.h"
#include "Button.h"
#include "Overlay.h"
#include "ImageWidget.h"
#include "Border.h"

namespace GuGu {
	ComboButton::ComboButton()
	{
		
	}
	ComboButton::~ComboButton()
	{
	}
	void ComboButton::init(const BuilderArguments& arguments)
	{
		std::shared_ptr<ButtonStyle> ourButtonStyle = arguments.mbuttonStyle.IsSet() ? arguments.mbuttonStyle.Get() : arguments.mcomboButtonStyle.Get()->m_buttonStyle;

		const bool bHasDownArrowShadow = !(arguments.mcomboButtonStyle.Get()->m_shadowOffset.x == 0 && arguments.mcomboButtonStyle.Get()->m_shadowOffset.y == 0);

		std::shared_ptr<HorizontalBox> hBox;

		MenuAnchor::init(
			MenuAnchor::BuilderArguments()
			.placement(arguments.mmenuPlacement)//弹出窗放置的位置
			.method(arguments.mmethod)//使用现有的窗口还是创建新窗口
			.onGetMenuContent(arguments.monGetMenuContent)//菜单内容
			.Content
			(
				//button and down arrow on the right
				//+---------------------+---+
				//| button content      | v |
				//+---------------------+---+
				WIDGET_NEW(Button)
				.buttonSyle(ourButtonStyle)
				.Clicked(this, &ComboButton::onButtonClicked)
				.contentPadding(arguments.mcontentPadding.IsSet() ? arguments.mcontentPadding : arguments.mcomboButtonStyle.Get()->m_contentPadding)
				.isFocusable(arguments.misFocusable)
				.Content
				(
					//按钮内容和下箭头
					WIDGET_ASSIGN_NEW(HorizontalBox, hBox)
					+ HorizontalBox::Slot()
					.expose(m_buttonContentSlot)
					.StretchWidth(1.0f)
					.setHorizontalAlignment(arguments.mhorizontalAlignment)
					.setVerticalAlignment(arguments.mverticalAlignment)
					(
						arguments.mbuttonContent->m_childWidget //按钮的内容
					)
					+ HorizontalBox::Slot()
					.FixedWidth()
					.setHorizontalAlignment(HorizontalAlignment::Right)
					.setVerticalAlignment(arguments.mhasDownArrow ? (VerticalAlignment)arguments.mcomboButtonStyle.Get()->m_downArrowAlign : VerticalAlignment::Center)
					.setPadding(arguments.mhasDownArrow ? arguments.mcomboButtonStyle.Get()->m_downArrowPadding : Padding(0.0f))
					(
						WIDGET_NEW(Overlay)
						//drop shadow
						+ Overlay::Slot()
						.setVerticalAlignment(VerticalAlignment::Top)
						.setHorizontalAlignment(HorizontalAlignment::Stretch)
						.setPadding(Padding(arguments.mcomboButtonStyle.Get()->m_shadowOffset.x, arguments.mcomboButtonStyle.Get()->m_shadowOffset.y, 0.0f, 0.0f))
						(
							WIDGET_NEW(ImageWidget)
							.visibility(arguments.mhasDownArrow && bHasDownArrowShadow ? Visibility::Visible : Visibility::Collapsed)
							.brush(arguments.mcomboButtonStyle.Get()->m_downArrowImage)//todo:add color and opacity
						)
						+ Overlay::Slot()
						.setVerticalAlignment(VerticalAlignment::Top)
						.setHorizontalAlignment(HorizontalAlignment::Stretch)
						(
							WIDGET_NEW(ImageWidget)
							.visibility(arguments.mhasDownArrow ? Visibility::Visible : Visibility::Collapsed)
							.brush(arguments.mcomboButtonStyle.Get()->m_downArrowImage)
						)
					)
				)
			)
		);

		setMenuContent(arguments.mmenuContent->getChildWidget());//菜单内容
	}
	bool ComboButton::shouldOpenDueToClick() const
	{
		//todo:Add  && !m_bDismissedThisTick
		return !isOpen();
	}
	void ComboButton::setMenuContent(std::shared_ptr<Widget> inMenuContent)
	{
		m_menuContent = m_wrappedContent =
			WIDGET_NEW(Border)
			.brush(m_menuBorderBrush)
			.padding(m_menuBorderPadding)
			.Content
			(
				inMenuContent
			);
	}
	Reply ComboButton::onButtonClicked() //这个函数会在按钮按下去的时候，调用
	{
		setIsOpen(shouldOpenDueToClick(), false);

		if (isOpen())
		{
			//todo:这里添加回调
		}

		Reply buttonClickedReply = Reply::Handled();

		std::shared_ptr<Widget> widgetToFocus = m_widgetToFocusPtr.lock();

		if (m_bIsFocusable)
		{
			if (!widgetToFocus)
			{
				if (m_wrappedContent && m_wrappedContent->getSlotsNumber() == 1)
				{
					widgetToFocus = m_wrappedContent->getSlot(0)->getChildWidget();
				}
			}

			if (!widgetToFocus)
			{
				widgetToFocus = m_menuContent;
			}

			if (!widgetToFocus)
			{
				widgetToFocus = m_contentWidgetPtr.lock();
			}
		}

		if (widgetToFocus)
		{
			buttonClickedReply.setFocus(widgetToFocus);
		}

		return buttonClickedReply;
	}
}