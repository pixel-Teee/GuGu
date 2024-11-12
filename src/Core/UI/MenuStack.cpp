#include <pch.h>

#include "MenuStack.h"
#include "WidgetPath.h"
#include "ArrangedWidget.h"
#include "Widget.h"
#include "Menu.h"

namespace GuGu {
	std::shared_ptr<IMenu> MenuStack::push(const WidgetPath& inOwnerPath, const std::shared_ptr<Widget>& inContent, const math::float2& summonLocation, const bool bFocusImmediately, const math::float2& summonLocationSize, std::optional<PopupMethod> inMethod, const bool bIsCollapsedByParent)
	{
		//return std::shared_ptr<IMenu>();

		math::box anchor(summonLocation, summonLocationSize);
		std::shared_ptr<IMenu> parentMenu;

		if (hasMenus())
		{
			//找到在InOwnerPath中的一个菜单，去决定什么等级插入
			//一个具体的菜单控件到逻辑上的MenuBase
			parentMenu = findMenuInWidgetPath(inOwnerPath);
		}

		if (!parentMenu)
		{
			//放入一个根root menu

			//active method 被决定，当一个新的根 root menu 被放入的时候
			//创建新窗口还是使用现在的窗口
			m_activeMethod = inMethod.has_value() ? PopupMethodReply::useMethod(inMethod.value()) : queryPopupMethod(inOwnerPath);
			
			//当一个新的根menu被push的时候，宿主窗口将被决定出来
			setHostPath(inOwnerPath);
		}

		return nullptr;
	}
	bool MenuStack::hasMenus() const
	{
		return m_stack.size() > 0;
	}
	std::shared_ptr<IMenu> MenuStack::findMenuInWidgetPath(const WidgetPath& pathToQuery) const
	{
		int32_t widgetNumber = pathToQuery.m_widgets.getArrangedWidgetsNumber();
		for (int32_t pathIndex = widgetNumber - 1; pathIndex >= 0; --pathIndex)
		{
			std::shared_ptr<Widget> widget = pathToQuery.m_widgets[pathIndex]->getWidget();
			
			auto it = m_cachedContentMap.find(widget);

			if (it != m_cachedContentMap.end())
			{
				return std::static_pointer_cast<IMenu>(it->second);
			}
		}

		return std::shared_ptr<IMenu>();
	}
	PopupMethodReply MenuStack::queryPopupMethod(const WidgetPath& pathToQuery)
	{
		int32_t widgetNumber = pathToQuery.m_widgets.getArrangedWidgetsNumber();
		for (int32_t widgetIndex = widgetNumber - 1; widgetIndex >= 0; --widgetIndex)
		{
			PopupMethodReply popupMethod = pathToQuery.m_widgets[widgetIndex]->getWidget()->OnQueryPopupMethod();
			if (popupMethod.isEventHandled())
			{
				return popupMethod;
			}
		}

		return PopupMethodReply::useMethod(PopupMethod::CreateNewWindow);
	}
	void MenuStack::setHostPath(const WidgetPath& inOwnerPath)
	{
		//todo:添加 host window guard
		

		hostWindow = inOwnerPath.isValid() ? inOwnerPath.getWindow() : std::shared_ptr<WindowWidget>();
		
		hostWidget = inOwnerPath.isValid() ? inOwnerPath.getLastWidget() : std::weak_ptr<Widget>();

		if (hostWindow)
		{

		}
	}
}