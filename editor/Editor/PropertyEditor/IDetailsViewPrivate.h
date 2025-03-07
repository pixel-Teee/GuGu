#pragma once

#include "IDetailsView.h"

namespace GuGu {
	class ComplexPropertyNode;
	struct DetailLayoutData;
	class IDetailsViewPrivate : public IDetailsView
	{
	public:
		virtual void updateSinglePropertyMap(std::shared_ptr<ComplexPropertyNode> inRootPropertyNode, DetailLayoutData& layoutData, bool bIsExternal) = 0;
	};
}