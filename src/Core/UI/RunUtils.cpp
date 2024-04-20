#include <pch.h>

#include "RunUtils.h"

namespace GuGu {
	TextRange calculateOffsetMeasureRange(const int32_t inOffset, const TextRange& inTextRange)
	{
		return TextRange(inTextRange.m_beginIndex, inTextRange.m_beginIndex + inOffset);
	}
}