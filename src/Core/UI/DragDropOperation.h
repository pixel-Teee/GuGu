#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class Widget;
	class WindowWidget;
	struct DragDropEvent;
	struct PointerEvent;

#define DRAG_DROP_OPERATOR_TYPE(TYPE, BASE) \
	static const GuGuUtf8Str& getTypeId() { static GuGuUtf8Str Type = #TYPE; return Type;}\
	virtual bool isOfTypeImpl(const GuGuUtf8Str& Type) const override { return getTypeId() == Type || BASE::isOfTypeImpl(Type);}

	class DragDropOperation : public std::enable_shared_from_this<DragDropOperation>
	{
	public:
		DragDropOperation() {}

		virtual ~DragDropOperation() {}

		template<class TType>
		bool isOfType() const 
		{
			return isOfTypeImpl(TType::getTypeId());
		}

		virtual std::shared_ptr<Widget> getDefaultDecorator() const;

		virtual void onDragged(const DragDropEvent& dragDropEvent);//移动装饰性窗口

		virtual void onDrop(bool bDropWasHandled, const PointerEvent& mouseEvent);

		virtual void destroyCursorDecoratorWindow();
	protected:
		virtual bool isOfTypeImpl(const GuGuUtf8Str& Type) const
		{
			return false;
		}

		virtual void construct();

		void createCursorDecoratorWindow();

	protected:
		//是否创建一个新窗口对于装饰内容
		bool m_bCreateNewWindow;

		//拥有装饰控件的窗口
		std::shared_ptr<WindowWidget> m_cursorDecoratorWindow;
	};
}