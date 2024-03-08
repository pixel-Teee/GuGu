#pragma once

#include <functional>

namespace GuGu {
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
	private:
		Reply(bool bIsHandled) : TReplyBase<Reply>(bIsHandled)
		{}
	};

	using OnClicked = std::function<Reply()>;
}