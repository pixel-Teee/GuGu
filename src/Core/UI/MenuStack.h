#pragma once

#include <Core/Math/MyMath.h>

#include "IMenu.h"
#include "PopupMethodReply.h"

#include <unordered_map>

namespace GuGu {
	class MenuBase;
	class WidgetPath;
	//打开的菜单的栈，栈中最后一个项表示最上面的菜单
	//菜单被描述为IMenus，IMenus的实现可以控制菜单如何创建和表示(例如，在他们自己的窗口)
	class MenuStack
	{
	public:
		MenuStack() {}

		std::shared_ptr<IMenu> push(const WidgetPath& inOwnerPath,
			const std::shared_ptr<Widget>& inContent,
			const math::float2& summonLocation,
			const bool bFocusImmediately = true,
			const math::float2& summonLocationSize = math::float2(0, 0),
			std::optional<PopupMethod> inMethod = std::optional<PopupMethod>(),
			const bool bIsCollapsedByParent = true);

		bool hasMenus() const;

		std::shared_ptr<IMenu> findMenuInWidgetPath(const WidgetPath& pathToQuery) const;

		PopupMethodReply queryPopupMethod(const WidgetPath& pathToQuery);

		void setHostPath(const WidgetPath& inOwnerPath);
	private:
		//被整个栈当前使用的 popup method ，同一时间只有一个
		PopupMethodReply m_activeMethod;

		//栈中的菜单数组
		std::vector<std::shared_ptr<MenuBase>> m_stack;

		//映射顶层的内容控件(应当总是MenuContentWrappers)到菜单在栈中
		std::unordered_map<std::shared_ptr<Widget>, std::shared_ptr<MenuBase>> m_cachedContentMap;

		//栈中根菜单的宿主，不是实际的菜单窗口，如果菜单是通过CreateNewWindow创建的
		std::shared_ptr<WindowWidget> hostWindow;

		//栈中根菜单的父亲
		std::weak_ptr<Widget> hostWidget;
	};
}