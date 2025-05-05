#pragma once

#include "IDetailsView.h"

namespace GuGu {
	class ComplexPropertyNode;
	struct DetailLayoutData;
	struct DetailColumnSizeData;
	class IDetailsViewPrivate : public IDetailsView
	{
	public:
		virtual void updateSinglePropertyMap(std::shared_ptr<ComplexPropertyNode> inRootPropertyNode, DetailLayoutData& layoutData, bool bIsExternal) = 0;

		virtual DetailColumnSizeData& getColumnSizeData() = 0;
	};
}