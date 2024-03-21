#pragma once

#include <functional>

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

	class Reply : public TReplyBase<Reply>
	{
	public:
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

		std::shared_ptr<Widget> getMouseCaptor()
		{
			return m_mouseCaptor.lock();
		}

		bool shouldReleaseMouse() const
		{
			return m_bReleaseMouseCapture;
		}
	private:
		Reply(bool bIsHandled) : TReplyBase<Reply>(bIsHandled)
		{}

		std::weak_ptr<Widget> m_mouseCaptor;

		bool m_bReleaseMouseCapture = false;
	};

	using OnClicked = std::function<Reply()>;
}