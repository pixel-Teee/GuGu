#include <pch.h>

#include "TextRange.h"
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	void TextRange::CalculateLineRangesFromString(const GuGuUtf8Str& input, std::vector<TextRange>& lineRanges)
	{
		//根据 \r\n 来切分字符串
		int32_t lineBeginInIndex = 0;

		for (size_t i = 0; i < input.len(); ++i)
		{
			GuGuUtf8Str Char = input[i];

			const bool bIsWindowsNewLine = (Char == "\r") && (input[i + 1] == "\n");
			if (bIsWindowsNewLine) //todo: add is line break
			{
				const int32_t lineEndIndex = i;
				lineRanges.emplace_back(TextRange(lineBeginInIndex, lineEndIndex));

				if (bIsWindowsNewLine)
				{
					++i;//skip the \n of \r\n chain
				}
				lineBeginInIndex = i + 1;
			}
		}

		if (lineBeginInIndex <= input.len())
		{
			lineRanges.emplace_back(TextRange(lineBeginInIndex, input.len()));
		}
	}
}