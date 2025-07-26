#pragma once

#include "IDetailChildrenBuilder.h"
#include "DetailCategoryBuilderImpl.h"//detail layout customization

namespace GuGu {
	class DetailCategoryImpl;
	class IDetailPropertyRow;
	class IPropertyHandle;
	class CustomChildrenBuilder : public IDetailChildrenBuilder
	{
	public:
		CustomChildrenBuilder(std::shared_ptr<DetailCategoryImpl> inParentCategory);

		virtual IDetailPropertyRow& addProperty(std::shared_ptr<IPropertyHandle> propertyHandle) override;

		const std::vector<DetailLayoutCustomization>& getChildCustomizations() const { return m_childCustomization; }
	private:
		std::weak_ptr<DetailCategoryImpl> m_parentCategory;

		std::vector<DetailLayoutCustomization> m_childCustomization;
	};
}