#pragma once

#include "PropertyNode.h"

namespace GuGu {
	class CategoryPropertyNode : public PropertyNode
	{
	public:
		CategoryPropertyNode();
		virtual ~CategoryPropertyNode();

		virtual CategoryPropertyNode* asCategoryNode() override { return this; }
		virtual const CategoryPropertyNode* asCategoryNode() const override { return this; }

		void setCategoryName(const GuGuUtf8Str& inCategoryName) { m_categoryName = inCategoryName; }
		const GuGuUtf8Str& getCategoryName() const { return m_categoryName; }
	protected:
		virtual void initChildNodes() override;

		GuGuUtf8Str m_categoryName;
	};
}