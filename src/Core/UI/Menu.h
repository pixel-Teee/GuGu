#pragma once

#include "IMenu.h"
#include "PopupMethodReply.h"

namespace GuGu {
	class Widget;
	class MenuBase : public IMenu, public std::enable_shared_from_this<MenuBase>
	{
	public:
		virtual std::shared_ptr<Widget> getContent() const override { return m_content; }

	protected:
		MenuBase(std::shared_ptr<Widget> inContent, const bool bCollapsedByParent);

		std::shared_ptr<Widget> m_content;
		bool m_bDismissing;
		bool m_bIsCollapsedByParent;
	};
	
	class WindowWidget;
	class MenuInHostWidget : public MenuBase
	{
	public:
		MenuInHostWidget(std::shared_ptr<IMenuHost> inHost, const std::shared_ptr<Widget>& inContent, const bool bIsCollapsedByParent);

		virtual ~MenuInHostWidget() {}

		virtual PopupMethod getPopupMethod() const { return PopupMethod::UseCurrentWindow; }
		virtual std::shared_ptr<WindowWidget> getParentWindow() const;
		virtual std::shared_ptr<WindowWidget> getOwnedWindow() const;

		virtual bool usingApplicationMenuStack() const override;
		virtual void dismiss() override;
	private:
		std::weak_ptr<IMenuHost> m_menuHost;
	};
}