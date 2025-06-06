#include <pch.h>

#include "Menu.h"
#include "WindowWidget.h"

namespace GuGu {
	MenuBase::MenuBase(std::shared_ptr<Widget> inContent, const bool bCollapsedByParent)
		: m_content(inContent)
		, m_bDismissing(false)
		, m_bIsCollapsedByParent(bCollapsedByParent)
	{
	}
	MenuInHostWidget::MenuInHostWidget(std::shared_ptr<IMenuHost> inHost, const std::shared_ptr<Widget>& inContent, const bool bIsCollapsedByParent)
		: MenuBase(inContent, bIsCollapsedByParent)
		, m_menuHost(inHost)
	{
	}
	std::shared_ptr<WindowWidget> MenuInHostWidget::getParentWindow() const
	{
		std::shared_ptr<IMenuHost> hostLocked = m_menuHost.lock();
		if (hostLocked)
		{
			return hostLocked->getMenuWindow();
		}

		return std::make_shared<WindowWidget>();
	}
	std::shared_ptr<WindowWidget> MenuInHostWidget::getOwnedWindow() const
	{
		return std::shared_ptr<WindowWidget>();
	}
	bool MenuInHostWidget::usingApplicationMenuStack() const
	{
		std::shared_ptr<IMenuHost> hostLocked = m_menuHost.lock();
		if (hostLocked)
		{
			return hostLocked->usingApplicationMenuStack();
		}

		return true;
	}
	void MenuInHostWidget::dismiss()
	{
		//todo:implement this
		if (!m_bDismissing)
		{
			m_bDismissing = true;
			std::shared_ptr<IMenuHost> hostLocked = m_menuHost.lock();
			if (hostLocked)
			{
				hostLocked->onMenuDismissed();
			}
		}
	}
	MenuInWindow::MenuInWindow(std::shared_ptr<WindowWidget> inWindow, std::shared_ptr<Widget> inContent, const bool bIsCollapsedByParent)
		: MenuBase(inContent, bIsCollapsedByParent)
		, m_window(inWindow)
	{
	}
	std::shared_ptr<WindowWidget> MenuInWindow::getParentWindow() const
	{
		return m_window.lock();
	}
	void MenuInWindow::dismiss()
	{
		//todo:implement this
		if (!m_bDismissing)
		{
			m_bDismissing = true;
		
			//关闭窗口
			std::shared_ptr<WindowWidget> windowLocked = m_window.lock();
			if (windowLocked)
			{
				windowLocked->requestDestroyWindow();
			}
		}
	}
}