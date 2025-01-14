#pragma once

namespace GuGu {
	class WindowWidget;
	class DragDropOperation : public std::enable_shared_from_this<DragDropOperation>
	{
	public:
		DragDropOperation() {}

		virtual ~DragDropOperation() {}

	protected:
		//是否创建一个新窗口对于装饰内容
		bool m_bCreateNewWindow;

		//拥有装饰控件的窗口
		std::shared_ptr<WindowWidget> m_cursorDecoratorWindow;
	};
}