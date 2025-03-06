#pragma once

#include <vector>


namespace GuGu {
	class ComplexPropertyNode;
	class DetailLayoutBuilderImpl;

	using RootPropertyNodeList = std::vector<std::shared_ptr<ComplexPropertyNode>>;

	struct DetailLayoutData
	{
		std::shared_ptr<DetailLayoutBuilderImpl> m_detailLayout;
	};

	using DetailLayoutList = std::vector<DetailLayoutData>;
}