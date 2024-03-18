#include "pch.h"

#include "PlainTextLayoutMarshaller.h"
#include "Style.h"
#include "GuGuTextLayout.h"
#include "TextRange.h"
#include "TextRun.h"

namespace GuGu {
	PlainTextLayoutMarshaller::PlainTextLayoutMarshaller()
	{
	}
	PlainTextLayoutMarshaller::~PlainTextLayoutMarshaller()
	{
	}
	void PlainTextLayoutMarshaller::setText(const GuGuUtf8Str& sourceString, TextLayout& targetTextLayout)
	{
		const TextBlockStyle& defaultTextStyle = static_cast<GuGuTextLayout&>(targetTextLayout).getDefaultTextStyle();

		std::vector<TextRange> lineRanges;
		TextRange::CalculateLineRangesFromString(sourceString, lineRanges);

		std::vector<TextLayout::NewLineData> linesToAdd;
		linesToAdd.reserve(lineRanges.size());

		const bool bUsePasswordRun = m_bIsPassword.Get();//todo:给予一个默认值

		for (int32_t lineIndex = 0; lineIndex < lineRanges.size(); ++lineIndex)
		{
			const TextRange& lineRange = lineRanges[lineIndex];
			std::shared_ptr<GuGuUtf8Str> lineText = std::make_shared<GuGuUtf8Str>(sourceString.substr(lineRange.m_beginIndex, lineRange.len()));

			std::vector<std::shared_ptr<IRun>> runs;

			//todo:增加密码IRun

			runs.push_back(TextRun::Create(RunInfo(), lineText, defaultTextStyle));

			linesToAdd.emplace_back(std::move(lineText), std::move(runs));
		}

		targetTextLayout.addLines(linesToAdd);
	}
	void PlainTextLayoutMarshaller::getText(GuGuUtf8Str& targetString, const TextLayout& sourceTextLayout)
	{
	}
	std::shared_ptr<PlainTextLayoutMarshaller> PlainTextLayoutMarshaller::Create()
	{
		return std::make_shared<PlainTextLayoutMarshaller>();
	}
}