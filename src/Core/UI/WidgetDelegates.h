#pragma once

#include <functional>

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	using OnIsTypedCharValid = std::function<bool(const GuGuUtf8Str)>;
	using OnFloatValueChanged = std::function<void(float)>;
}