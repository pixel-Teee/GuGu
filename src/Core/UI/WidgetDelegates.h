#pragma once

#include <functional>

#include <Core/GuGuUtf8Str.h>

#include "BasicElement.h"

namespace GuGu {
	using OnIsTypedCharValid = std::function<bool(const GuGuUtf8Str)>;
	using OnFloatValueChanged = std::function<void(float)>;
	using OnTextCommitted = std::function<void(const GuGuUtf8Str&, TextCommit::Type)>;

	class ITableRow;
	class TableViewBase;
	template<typename ArgumentType>
	class WidgetDelegates
	{
	public:
		using OnGenerateRow = std::function<std::shared_ptr<ITableRow>(ArgumentType, const std::shared_ptr<class TableViewBase>&)>;

		using OnGetChildren = std::function<void(ArgumentType, std::vector<ArgumentType>&)>;
	};
}