#pragma once

#include <vector>
#include <map>
#include <Core/GuGuUtf8Str.h>//string

namespace GuGu {
	class ComplexPropertyNode;
	class DetailLayoutBuilderImpl;
	class DetailCategoryImpl;

	using RootPropertyNodeList = std::vector<std::shared_ptr<ComplexPropertyNode>>;

	using CategoryMap = std::map<GuGuUtf8Str, std::shared_ptr<DetailCategoryImpl>>;

	struct DetailLayoutData
	{
		std::shared_ptr<DetailLayoutBuilderImpl> m_detailLayout;
	};

	using DetailLayoutList = std::vector<DetailLayoutData>;
}