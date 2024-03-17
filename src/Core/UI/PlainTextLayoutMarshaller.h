#include "ITextLayoutMarshaller.h"

namespace GuGu {
	class PlainTextLayoutMarshaller : public ITextLayoutMarshaller
	{
	public:
		PlainTextLayoutMarshaller();

		virtual ~PlainTextLayoutMarshaller();

		virtual void setText(const GuGuUtf8Str& sourceString, TextLayout& targetTextLayout) override;
		
		virtual void getText(GuGuUtf8Str& targetString, const TextLayout& sourceTextLayout) override;

		static std::shared_ptr<PlainTextLayoutMarshaller> Create();
	};
}