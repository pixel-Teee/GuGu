#pragma once

#include "Reply.h"

namespace GuGu {
	enum class PopupMethod : uint8_t
	{
		//创建一个新窗口放置在窗口的外面，也就是 menu anchor 停放的地方
		CreateNewWindow,
		//防止 pop up 在现在的窗口，如果应用目的是运行全屏，则不能窗口新窗口，所以必须使用这个方法
		UseCurrentWindow
	};

	//reply 通知是否应当表示 popup，通过创建一个新窗口或者重复使用现在的窗口
	class PopupMethodReply : public TReplyBase<PopupMethodReply>
	{
	public:
		PopupMethodReply()
			: TReplyBase<PopupMethodReply>(false)
			, m_popupMethod(PopupMethod::CreateNewWindow)
		{}

		static PopupMethodReply unHandled()
		{
			return PopupMethodReply(false, PopupMethod::CreateNewWindow);
		}

		static PopupMethodReply useMethod(PopupMethod withMethod)
		{
			return PopupMethodReply(true, withMethod);
		}

		bool isSet() const
		{
			return isEventHandled();
		}

		PopupMethod getPopupMethod() const
		{
			return m_popupMethod;
		}
	private:
		PopupMethodReply(bool bIsHandled, PopupMethod inMethod)
			: TReplyBase<PopupMethodReply>(bIsHandled)
			, m_popupMethod(inMethod)
		{}
	private:
		PopupMethod m_popupMethod;
	};
}