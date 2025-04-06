#pragma once

#include "PropertyNode.h"

namespace GuGu {
	class ItemPropertyNode : public PropertyNode
	{
	public:
		ItemPropertyNode();
		virtual ~ItemPropertyNode();

		virtual ItemPropertyNode* asItemPropertyNode() override { return this; }
		virtual const ItemPropertyNode* asItemPropertyNode() const override { return this; }

		virtual GuGuUtf8Str getDisplayName() const override;
	protected:
		virtual void initChildNodes() override;
	};
}