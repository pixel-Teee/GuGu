#pragma once

#include "DetailLayoutBuilder.h"
#include "DetailTreeNode.h" //DetailNodeList
#include "PropertyRowGenerator.h"//CategoryMap

namespace GuGu {
	class DetailLayoutBuilderImpl : public IDetailLayoutBuilder
	{
	public:
		DetailLayoutBuilderImpl();

		virtual ~DetailLayoutBuilderImpl();

		//构造 tree view 的 detail node 的数组
		DetailNodeList& getFilteredRootTreeNodes() { return m_filteredRootTreeNodes;}

		void generateDetailLayout();

	private:
		DetailNodeList m_filteredRootTreeNodes;

		DetailNodeList m_allRootTreeNodes;

		CategoryMap m_customCategoryMap;

		std::weak_ptr<ComplexPropertyNode> m_rootNode;
	};
}