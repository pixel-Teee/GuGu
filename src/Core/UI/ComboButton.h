#pragma once

#include "MenuAnchor.h"
#include "BoxPanel.h"
#include "UIMacros.h"
#include "CoreStyle.h"

namespace GuGu {
	//一个按钮，当点击的时候，弹出一个 pop up
	class ComboButton : public MenuAnchor
	{
	public:
		ComboButton();

		virtual ~ComboButton();

		struct BuilderArguments : public Arguments<ComboButton>
		{
			BuilderArguments()
				: mcomboButtonStyle(CoreStyle::getStyleSet()->template getStyle<ComboButtonStyle>("comboButton"))
				, mmenuPlacement(MenuPlacementComboBox)
				, misFocusable(true)
				, mhorizontalAlignment(HorizontalAlignment::Stretch)
				, mverticalAlignment(VerticalAlignment::Center)
				, mhasDownArrow(true)
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ComboButtonStyle>, comboButtonStyle)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ButtonStyle>, buttonStyle) //覆盖 combo button style 的 button style

			UI_EVENT(OnGetContent, onGetMenuContent)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, buttonContent)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, menuContent)

			ARGUMENT_VALUE(bool, isFocusable)
			ARGUMENT_VALUE(bool, hasDownArrow)

			ARGUMENT_ATTRIBUTE(MenuPlacement, menuPlacement)
			ARGUMENT_ATTRIBUTE(Padding, contentPadding)

			ARGUMENT_VALUE(std::optional<PopupMethod>, method)

			ARGUMENT_VALUE(HorizontalAlignment, horizontalAlignment)
			ARGUMENT_VALUE(VerticalAlignment, verticalAlignment)
		};

		void init(const BuilderArguments& arguments);

		bool shouldOpenDueToClick() const;

		virtual void setMenuContent(std::shared_ptr<Widget> inMenuContent);
	protected:
		virtual Reply onButtonClicked();
	protected:
		//按钮内容放置的区域
		HorizontalBox::HorizontalBoxSlot* m_buttonContentSlot;

		//当 combo list 被打开的时候去执行的委托

		std::weak_ptr<Widget> m_widgetToFocusPtr;

		Padding m_menuBorderPadding;

		//围绕在下拉列表内容周围的 menu border 的画刷
		std::shared_ptr<Brush> m_menuBorderBrush;

		//内容 widget
		std::weak_ptr<Widget> m_contentWidgetPtr;

		//这个按钮可以被聚焦吗？
		bool m_bIsFocusable;
	};
}