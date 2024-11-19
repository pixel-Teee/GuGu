#pragma once

#include <Core/Math/MyMath.h>

#include "IMenu.h"
#include "PopupMethodReply.h"
#include "BasicElement.h" //optional size

#include <unordered_map>

namespace GuGu {
	class MenuBase;
	class WidgetPath;
	class PopupLayer;
	class MenuPanel;
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

		void onMenuContentLostFocus(const WidgetPath& inFocussedPath);

		//销毁菜单以及他的所有子菜单
		void dimissFrom(const std::shared_ptr<IMenu>& inFromMenu);

		//解散所有菜单栈
		void dimissAll();

		void dimissInternal(int32_t firstStackIndexToRemove);

		std::shared_ptr<IMenu> pushInternal(const std::shared_ptr<IMenu>& inParentMenu, const std::shared_ptr<Widget>& inContent, math::box2 anchor, const bool bFocusImmediately, const bool bIsCollapsedByParent);
	private:
		struct PrePushArgs
		{
			PrePushArgs() {}

			std::shared_ptr<Widget> m_content;
			math::box2 m_anchor;
			bool m_bFocusImmediately;
			bool m_bIsCollapsedByParent;
		};

		struct PrePushResults
		{
			std::shared_ptr<Widget> m_warppedContent;
			std::shared_ptr<Widget> m_widgetToFocus;
			math::float2 m_startLocation;
			math::float2 m_expectedSize;
			bool m_bFocusImmediately;
			bool m_bIsCollapsedByParent;
		};

		PrePushResults prePush(const PrePushArgs& inArgs);

		std::shared_ptr<MenuBase> pushNewWindow(std::shared_ptr<IMenu> inParentMenu, const PrePushResults& inPrePushResults);

		std::shared_ptr<MenuBase> pushPopup(std::shared_ptr<IMenu> inParentMenu, const PrePushResults& inPrePushResults);

		void postPush(std::shared_ptr<IMenu> inParentMenu, std::shared_ptr<MenuBase> inMenu, bool bInInsertAfterDismiss = false);

		std::shared_ptr<Widget> wrapContent(std::shared_ptr<Widget> inContent, OptionalSize optionalMinWidth = OptionalSize(), OptionalSize optionalMinHeight = OptionalSize());

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

		//包含我们的HostWindowPopupPanel的popup layer
		std::shared_ptr<PopupLayer> m_hostPopupLayer;

		//在栈中根菜单的父窗口，不是实际的菜单窗口，如果它从CreateNewWindow创建
		std::shared_ptr<MenuPanel> m_hostWindowPopupPanel;

		std::shared_ptr<MenuBase> m_pendingNewMenu;
	};
}