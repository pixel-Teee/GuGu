#pragma once

#include "TextRange.h"

namespace GuGu {
	/*
	* 计算将测量给定index的range
	*/
	TextRange calculateOffsetMeasureRange(const int32_t inOffset, const TextRange& inTextRange);
}