#pragma once

#include <vector>

#include <Core/HashCombine.h>

namespace GuGu {
	class GuGuUtf8Str;

	struct TextRange
	{
		TextRange()
			: m_beginIndex(-1)
			, m_endIndex(-1)
		{}

		TextRange(int32_t inBeginIndex, int32_t inEndIndex)
			: m_beginIndex(inBeginIndex), m_endIndex(inEndIndex)
		{}

		int32_t len() const { return m_endIndex - m_beginIndex; }
		bool isEmpty() const { return (m_endIndex - m_beginIndex) <= 0; }

		static void CalculateLineRangesFromString(const GuGuUtf8Str& input, std::vector<TextRange>& lineRanges);

		bool Contains(int32_t index) const { return index >= m_beginIndex && index <= m_endIndex; }

		void offset(int32_t amount) { m_beginIndex += amount; m_beginIndex = std::max(0, m_beginIndex); m_endIndex += amount; m_endIndex += std::max(0, m_endIndex); }

		bool inculsiveContains(int32_t index) const { return index >= m_beginIndex && index <= m_endIndex; }

		TextRange intersect(const TextRange& other) const
		{
			TextRange intersected(std::max(m_beginIndex, other.m_beginIndex), std::min(m_endIndex, other.m_endIndex));

			if (intersected.m_endIndex <= intersected.m_beginIndex)
				return TextRange(0, 0);

			return intersected;
		}

		bool operator==(const TextRange& other) const
		{
			return m_beginIndex == other.m_beginIndex && m_endIndex == other.m_endIndex;
		}
		int32_t m_beginIndex;
		int32_t m_endIndex;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::TextRange>
	{
		size_t operator()(const GuGu::TextRange& s) const
		{
			size_t keyHash = 0;
			GuGu::hash_combine(keyHash, s.m_beginIndex);
			GuGu::hash_combine(keyHash, s.m_endIndex);
			return keyHash;
		}
	};
}