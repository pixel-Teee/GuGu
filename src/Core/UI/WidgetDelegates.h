#pragma once

#include <functional>

#include <Core/GuGuUtf8Str.h>

#include "BasicElement.h"

namespace GuGu {
	using OnIsTypedCharValid = std::function<bool(const GuGuUtf8Str)>;
	using OnFloatValueChanged = std::function<void(float)>;
	using OnTextCommitted = std::function<void(const GuGuUtf8Str&, TextCommit::Type)>;
}