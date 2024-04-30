#include <pch.h>

#include "Menu.h"
#include "WindowWidget.h"

namespace GuGu {
	MenuBase::MenuBase(std::shared_ptr<Widget> inContent, const bool bCollapsedByParent)
		: m_content(inContent)
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
	}
}