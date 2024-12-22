#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	struct ContentBrowserItem
	{
	public:
		GuGuUtf8Str getPath() const;//invariant path(不可变路径)，不管触发了什么过滤逻辑？

		GuGuUtf8Str getItemName() const;
	};
}