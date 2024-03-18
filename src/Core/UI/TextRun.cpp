#include <pch.h>

#include "TextRun.h"

namespace GuGu {
	TextRun::TextRun(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style)
		: m_runInfo(inRunInfo)
		, m_text(inText)
		, m_style(style)
		, m_range(0, inText->len())
	{
	}
	TextRun::~TextRun()
	{
	}
	TextRange TextRun::getTextRange() const
	{
		return m_range;
	}
	std::shared_ptr<TextRun> TextRun::Create(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style)
	{
		return std::make_shared<TextRun>(inRunInfo, inText, style);
	}
}