#include "pch.h"

#include "PlainTextLayoutMarshaller.h"

namespace GuGu {
	PlainTextLayoutMarshaller::PlainTextLayoutMarshaller()
	{
	}
	PlainTextLayoutMarshaller::~PlainTextLayoutMarshaller()
	{
	}
	void PlainTextLayoutMarshaller::setText(const GuGuUtf8Str& sourceString, TextLayout& targetTextLayout)
	{
	}
	void PlainTextLayoutMarshaller::getText(GuGuUtf8Str& targetString, const TextLayout& sourceTextLayout)
	{
	}
	std::shared_ptr<PlainTextLayoutMarshaller> PlainTextLayoutMarshaller::Create()
	{
		return std::make_shared<PlainTextLayoutMarshaller>();
	}
}