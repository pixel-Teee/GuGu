#pragma once

#include <functional>
#include <Application/InputTypes.h>

namespace GuGu {
	class Widget;
	class ReplyBase
	{
	public:
		bool isEventHandled() const { return m_bIsHandled; }
	protected:
		ReplyBase(bool inIsHandled)
			: m_bIsHandled(inIsHandled)
		{}
		bool m_bIsHandled;
	};
	template<typename ReplyType>
	class TReplyBase : public ReplyBase
	{
	public:
		TReplyBase(bool bIsHandled)
			: ReplyBase(bIsHandled)
		{}
	protected:
		ReplyType& Me()
		{
			return static_cast<ReplyType&>(*this);
		}
	};
	class DragDropOperation;
	class Reply : public TReplyBase<Reply>
	{
	public:
		Reply()
		: TReplyBase<Reply>(false)
		{

		}
		
		static Reply Handled()
		{
			return Reply(true);
		}

		static Reply Unhandled()
		{
			return Reply(false);
		}

		Reply& captureMouse(std::shared_ptr<Widget> inMouseCaptor)
		{
			m_bReleaseMouseCapture = false;
			m_mouseCaptor = inMouseCaptor;
			return Me();
		}

		Reply& releaseMouseCapture()
		{
			m_bReleaseMouseCapture = true;
			m_mouseCaptor.reset();
			return Me();
		}

		Reply& detectDrag(const std::shared_ptr<Widget>& detectDragInMe, Key mouseButton)
		{
			this->m_detectDragForWidget = detectDragInMe;
			this->m_detectDragForMouseButton = mouseButton;
			return Me();
		}

		//返回检测拖拽的控件
		std::shared_ptr<Widget> getDetectDragRequest() const { return m_detectDragForWidget.lock(); }

		Key getDetectDragRequestButton() const { return m_detectDragForMouseButton; }

		std::shared_ptr<Widget> getMouseCaptor() const
		{
			return m_mouseCaptor.lock();
		}

		bool shouldReleaseMouse() const
		{
			return m_bReleaseMouseCapture;
		}

		Reply& setFocus(std::shared_ptr<Widget> giveMeFocus);

		std::shared_ptr<Widget> getFocusRecepient() const { return m_focusRecipient.lock(); }

		std::shared_ptr<DragDropOperation> getDragDropContent() const { return m_dragDropContent; }
	private:
		Reply(bool bIsHandled) : TReplyBase<Reply>(bIsHandled)
		{}

		std::weak_ptr<Widget> m_mouseCaptor;

		std::weak_ptr<Widget> m_focusRecipient;

		bool m_bReleaseMouseCapture = false;

		uint32_t m_bSetUserFocus : 1;

		std::weak_ptr<Widget> m_detectDragForWidget;
		Key m_detectDragForMouseButton;

		std::shared_ptr<DragDropOperation> m_dragDropContent;
	};

	using OnClicked = std::function<Reply()>;
}