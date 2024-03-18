#pragma once

#include "IRun.h"

#include <memory>

#include "Style.h"//TextBlockStyle
#include "TextRange.h"

namespace GuGu {
	//class TextBlockStyle;
	class TextRun : public IRun
	{
	public:
		TextRun(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style);

		virtual ~TextRun();

		virtual TextRange getTextRange() const override;

		static std::shared_ptr<TextRun> Create(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style);
	protected:
		RunInfo m_runInfo;
		std::shared_ptr<const GuGuUtf8Str> m_text;
		TextBlockStyle m_style;
		TextRange m_range;
	};
}