#pragma once

namespace GuGu {
	class Widget;
	class IToolTip
	{
	public:
		virtual ~IToolTip() {}

		//这个 tool tip 所表示的控件
		virtual std::shared_ptr<Widget> asWidget() = 0;

		//这个 tool tip 的内容控件
		virtual std::shared_ptr<Widget> getContentWidget() = 0;

		virtual void setContentWidget(const std::shared_ptr<Widget>& inContentWidget) = 0;

		//检查这个 tool tip 有没有内容
		virtual bool isEmpty() const = 0;

		//请求打开这个 tool tip
		virtual void onOpening() = 0;

		//关闭这个 tool tip
		virtual void onClosed() = 0;
	};
}