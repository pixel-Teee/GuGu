#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class IDetailLayoutRow
	{
		virtual GuGuUtf8Str getRowName() const = 0;
	};

	//表示 details view 的一个目录
	class IDetailCategoryBuilder
	{
	public:
		virtual ~IDetailCategoryBuilder() {}

		virtual const GuGuUtf8Str& getDisplayName() const = 0;
	};
}