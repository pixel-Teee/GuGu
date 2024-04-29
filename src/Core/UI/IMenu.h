#pragma once

namespace GuGu {
	class Widget;
	class WindowWidget;
	enum class PopupMethod : uint8_t;
	//表示一个 pop up 菜单
	class IMenu
	{
	public:
		virtual ~IMenu() {}
		virtual PopupMethod getPopupMethod() const = 0;
		virtual std::shared_ptr<WindowWidget> getParentWindow() const = 0;
		virtual std::shared_ptr<WindowWidget> getOwnedWindow() const = 0;
		virtual std::shared_ptr<Widget> getContent() const = 0;

		virtual bool usingApplicationMenuStack() const = 0;
		virtual void dismiss() = 0;
	};
	
	class IMenuHost//菜单的宿主
	{
	public:
		virtual std::shared_ptr<WindowWidget> getMenuWindow() const = 0;//宿主窗口
		virtual void onMenuDismissed() = 0;//当菜单消失的时候
		virtual bool usingApplicationMenuStack() const = 0;//是否使用应用的菜单栈
	};
}