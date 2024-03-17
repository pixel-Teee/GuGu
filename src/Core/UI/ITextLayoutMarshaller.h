#pragma once

namespace GuGu {
	class TextLayout;
	class GuGuUtf8Str;
	class ITextLayoutMarshaller
	{
	public:
		virtual ~ITextLayoutMarshaller() {}

		virtual void setText(const GuGuUtf8Str& sourceString, TextLayout& targetTextLayout) = 0;

		virtual void getText(GuGuUtf8Str& targetString, const TextLayout& sourceTextLayout) = 0;
	};
}