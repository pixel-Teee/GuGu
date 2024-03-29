#pragma once

#include "TextRange.h"

namespace GuGu {
	class ILineHighlighter;

	struct TextLineHighlight
	{
		TextLineHighlight(int32_t inLineIndex, const TextRange& inRange, int32_t inZorder, const std::shared_ptr
		<ILineHighlighter>& inHighlighter)
			: m_lineIdex(inLineIndex)
			, m_range(inRange)
			, m_zorder(inZorder)
			, m_highLighter(inHighlighter)
		{}

		bool operator==(const TextLineHighlight& other) const
		{
			return m_lineIdex == other.m_lineIdex
				&& m_range == other.m_range
				&& m_zorder == other.m_zorder
				&& m_highLighter == other.m_highLighter;
		}
		int32_t m_lineIdex;
		TextRange m_range;
		int32_t m_zorder;
		std::shared_ptr<ILineHighlighter> m_highLighter;
	};
}